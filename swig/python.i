%module murkpy

%include <stl.i>

%typemap(in) std::string_view %{
  {
    auto* obj = PyObject_Repr($input);
    $1 = std::string_view(PyString_AsString(obj), PyString_Size(obj));
  }
%}

%typemap(out) std::string_view %{
  $1 = PyString_FromStringAndSize($1.data(), $1.size());
%}


%typemap(typecheck,precedence=SWIG_TYPECHECK_STRING) std::string_view {
  $1 = PyString_Check($input) ? 1 : 0;
}

%{
  #define SWIG_FILE_WITH_INIT
  #include "murk/priv/docker.hpp"
%}
%include "murk/priv/docker.hpp"
