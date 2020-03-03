%module murkpy
#pragma SWIG nowarn=503

%include <stl.i>
%include <std_string_view.i>

%{
  #define SWIG_FILE_WITH_INIT
  #include "murk/baby.hpp"
%}

%include "murk/baby.hpp"
