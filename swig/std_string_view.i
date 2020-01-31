%{
  #include <string>
%}

%typemap(in) std::string_view %{
  auto* obj = PyObject_Repr($input);
  $1 = std::string_view(PyString_AsString(obj), PyString_Size(obj));
%}

abort();
