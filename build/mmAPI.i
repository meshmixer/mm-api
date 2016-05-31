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

namespace mm {
%extend wrapped_array {
  inline size_t __len__() const { return N; }
  inline const Type& __getitem__(size_t i) const throw(std::out_of_range) {
    if (i >= N || i < 0)
      throw std::out_of_range("out of bounds access");
    return self->data[i];
  }
  inline void __setitem__(size_t i, const Type& v) throw(std::out_of_range) {
    if (i >= N || i < 0)
      throw std::out_of_range("out of bounds access");
    self->data[i] = v;
  }
}
%template (floatArray9) wrapped_array<float, 9>;
}