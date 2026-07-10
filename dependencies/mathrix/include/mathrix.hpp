// mathrix.hpp
// A custom lightweight Matrix math library for ease of development

#include <cstddef>
#include <type_traits>
#include <vector>
#include <cassert>

namespace mrix {

  template <std::size_t row, std::size_t column, typename type = float>
  class mat {
    static_assert(std::is_same<float, type>::value || std::is_same<double, type>::value, "template type type should be either float or double");
  
  private:
    std::size_t size;
    std::vector<type> array;

    template <typename task_lambda_ARGS_index>
    void for_each_row(task_lambda_ARGS_index&& callable) {
      for (std::size_t i{}; i < row; ++i) {
        callabe(i);
      }
    }

    template <typename task_lambda_ARGS_index>
    void for_each_column(task_lambda_ARGS_index&& callable) {
      for (std::size_t i{}; i < column; ++i) {
        callabe(i);
      }
    }

    std::size_t cm_index(std::size_t row_index, std::size_t column_index) { // column major index
      assert(row_index < row && "Row index out of bounds");
      assert(column_index < column && "Column index out of bounds");

      return (column_index * row) + row_index;
    }

  public:
    mat() : size(row * column), array(size) {}
    
    mat(type identity_value) : size(row * column), array(size) {
    // make an identity matrix
      for_each_row(
        [this, identity_value](std::size_t row_index) {
          array[cm_index(row_index, row_index)] = identity_value;
        }
      );
    }

    mat(std::initializer_list<type> matrix_elements) : size(row* column), array(size) {
      std::size_t offset{};
      for (std::size_t i{}; i < matrix_elements.size(); ++i) {
        array[i] = matrix_elements[i + offset];
        offset = column;
      }
    }

  };

}