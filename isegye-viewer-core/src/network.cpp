// Copyright 2024 kus-system-security-team-1
#include "..\include\network.h"

Network::Network() {
}

static const GUID FWPM_CONDITION_ALE_PROCESS_ID_GUID = { 0x1ec1b7c9,0x4ee6,0x45b5,{0x9f,0x0f,0xd0,0xda,0x35,0x4f,0xdb,0x05} };
static GUID subLayerKey = { 0x11111111,0x1111,0x1111,{0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11} };
static std::map<DWORD, std::pair<UINT64, UINT64>> pidFilterMap;

int Network::blockProcessTraffic(DWORD pid) {
    wchar_t name[] = L"BlockSubLayer";
    HANDLE engineHandle = NULL;
    FWPM_SESSION session = { 0 };
    if (FwpmEngineOpen(NULL, RPC_C_AUTHN_WINNT, NULL, &session, &engineHandle) != ERROR_SUCCESS)return 1;
    FWPM_SUBLAYER subLayer = { 0 };
    subLayer.subLayerKey = subLayerKey;
    subLayer.displayData.name = name;
    subLayer.weight = 0x100;
    FwpmSubLayerAdd(engineHandle, &subLayer, NULL);
    UINT64 pidVal = pid;
    FWPM_FILTER_CONDITION cond[1];
    cond[0].fieldKey = FWPM_CONDITION_ALE_PROCESS_ID_GUID;
    cond[0].matchType = FWP_MATCH_EQUAL;
    cond[0].conditionValue.type = FWP_UINT64;
    cond[0].conditionValue.uint64 = &pidVal;
    FWPM_FILTER outboundFilter = { 0 };
    outboundFilter.layerKey = FWPM_LAYER_ALE_AUTH_CONNECT_V4;
    outboundFilter.subLayerKey = subLayerKey;
    outboundFilter.weight.type = FWP_EMPTY;
    outboundFilter.action.type = FWP_ACTION_BLOCK;
    outboundFilter.numFilterConditions = 1;
    outboundFilter.filterCondition = cond;
    UINT64 outboundId = 0;
    if (FwpmFilterAdd(engineHandle, &outboundFilter, NULL, &outboundId) != ERROR_SUCCESS) { FwpmEngineClose(engineHandle); return 1; }
    FWPM_FILTER inboundFilter = { 0 };
    inboundFilter.layerKey = FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4;
    inboundFilter.subLayerKey = subLayerKey;
    inboundFilter.weight.type = FWP_EMPTY;
    inboundFilter.action.type = FWP_ACTION_BLOCK;
    inboundFilter.numFilterConditions = 1;
    inboundFilter.filterCondition = cond;
    UINT64 inboundId = 0;
    if (FwpmFilterAdd(engineHandle, &inboundFilter, NULL, &inboundId) != ERROR_SUCCESS) { FwpmEngineClose(engineHandle); return 1; }
    pidFilterMap[pid] = std::make_pair(outboundId, inboundId);
    FwpmEngineClose(engineHandle);
    return 0;
}

int Network::unblockProcessTraffic(DWORD pid) {
    HANDLE engineHandle = NULL;
    FWPM_SESSION session = { 0 };
    if (FwpmEngineOpen(NULL, RPC_C_AUTHN_WINNT, NULL, &session, &engineHandle) != ERROR_SUCCESS)return 1;
    auto it = pidFilterMap.find(pid);
    if (it == pidFilterMap.end()) { FwpmEngineClose(engineHandle); return 1; }
    FwpmFilterDeleteById(engineHandle, it->second.first);
    FwpmFilterDeleteById(engineHandle, it->second.second);
    pidFilterMap.erase(it);
    bool hasMore = false;
    for (auto& kv : pidFilterMap) { hasMore = true; break; }
    if (!hasMore)FwpmSubLayerDeleteByKey(engineHandle, &subLayerKey);
    FwpmEngineClose(engineHandle);
    return 0;
}

void bind_network(py::module_& m) {
    py::class_<Network>(m, "Network")
        .def(py::init())
        .def("blockProcessTraffic", &Network::blockProcessTraffic)
        .def("unblockProcessTraffic", &Network::unblockProcessTraffic);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}