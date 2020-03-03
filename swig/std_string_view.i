%typemap(in) std::string_view %{
#define TMPNAME $1 ## _tmp
  PyObject* TMPNAME;
  TMPNAME = PyUnicode_AsUTF8String($input);
  $1 = std::string_view{PyBytes_AsString(TMPNAME), static_cast<size_t>(PyBytes_Size(TMPNAME))};
#undef TMPNAME
%}

%typemap(freearg) std::string_view %{
#define TMPNAME $1 ## _tmp
  Py_DECREF(TMPNAME);
#undef TMPNAME
%}

%typemap(out) std::string_view %{
  $1 = PyBytes_FromStringAndSize($1.data(), $1.size());
%}


%typemap(typecheck,precedence=SWIG_TYPECHECK_STRING) std::string_view {
  $1 = PyUnicode_Check($input);
}
