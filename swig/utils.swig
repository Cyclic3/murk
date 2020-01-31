%module murk
%{
  #define SWIG_FILE_WITH_INIT
  #include "murk/priv/docker.hpp"
%}
%include "murk/priv/docker.hpp"
