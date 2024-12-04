#include <pybind11/pybind11.h>

namespace py = pybind11;

void bind_history(py::module_& m);
void bind_pe(py::module_& m);
void bind_proc_info(py::module_& m);

PYBIND11_MODULE(isegye_viewer_core, m) {
    m.doc() = "isegye_viewer_core module: combines proc_info and pe functionality";
    
    bind_history(m);
    bind_pe(m);
    bind_proc_info(m);
}