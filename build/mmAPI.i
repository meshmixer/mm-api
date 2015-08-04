%module mmapi;

%{
#include "BinarySerializer.h"
#include "StoredCommands.h"
%}

%include <std_string.i>
%include <std_vector.i>
namespace std {
   %template(vectorub) vector<unsigned char>;
   %template(vectori) vector<int>;
   %template(vectorui) vector<unsigned int>;
   %template(vectorf) vector<float>;
   %template(vectorBrushStamp) vector<mm::brush_stamp>;
};

%include <carrays.i>
%array_class(float, floatArray);
%apply(unsigned char *buffer, size_t len) { (unsigned char *str, size_t len) };
%array_class(int, intArray);


%include "BinarySerializer.h"
%include "StoredCommands.h"

