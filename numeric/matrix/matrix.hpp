/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                 *
 * ALPS Project: Algorithms and Libraries for Physics Simulations                  *
 *                                                                                 *
 * ALPS Libraries                                                                  *
 *                                                                                 *
 * Copyright (C) 2010 - 2012 by Andreas Hehn <hehn@phys.ethz.ch>                   *
 *                                                                                 *
 * This software is part of the ALPS libraries, published under the ALPS           *
 * Library License; you can use, redistribute it and/or modify it under            *
 * the terms of the license, either version 1 or (at your option) any later        *
 * version.                                                                        *
 *                                                                                 *
 * You should have received a copy of the ALPS Library License along with          *
 * the ALPS Libraries; see the file LICENSE.txt. If not, the license is also       *
 * available from http://alps.comp-phys.org/.                                      *
 *                                                                                 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT       *
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE       *
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,     *
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER     *
 * DEALINGS IN THE SOFTWARE.                                                       *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef ALPS_MATRIX_HPP
#define ALPS_MATRIX_HPP

#include <alps/numeric/matrix/strided_iterator.hpp>
#include <alps/numeric/matrix/matrix_element_iterator.hpp>
#include <alps/numeric/matrix/vector.hpp>
#include <alps/numeric/matrix/detail/matrix_adaptor.hpp>
#include <alps/numeric/matrix/matrix_traits.hpp>
#include <alps/numeric/matrix/matrix_interface.hpp>
#include <alps/numeric/matrix/matrix_matrix_multiply.hpp>
#include <alps/numeric/real.hpp>
#include <alps/parser/xmlstream.h>
#include <boost/utility/enable_if.hpp>

#include <ostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cassert>


namespace alps {
    namespace numeric {
    /** A matrix template class
      *
      * The matrix class is a matrix which can take any type T
      * @param T the type of the elements to be stored in the matrix
      * @param MemoryBlock the underlying (continous) Memory structure
      */
    template <typename T, typename MemoryBlock = std::vector<T> >
    class matrix {
    public:
        // typedefs required for a std::container concept
        typedef T                       value_type;       // The type T of the elements of the matrix
        typedef T&                      reference;        // Reference to value_type
        typedef T const&                const_reference;  // Const reference to value_type
        typedef std::size_t             size_type;        // Unsigned integer type that represents the dimensions of the matrix
        typedef std::ptrdiff_t          difference_type;  // Signed integer type to represent the distance of two elements in the memory

        // for compliance with an std::container one would also need
        // -operators == != < > <= >=
        // -size()
        // -typedefs iterator, const_iterator

        // typedefs for matrix specific iterators
        typedef strided_iterator<matrix,value_type>
            row_element_iterator;                         // Iterator to iterate through the elements of a row of the matrix
        typedef strided_iterator<const matrix,const value_type>
            const_row_element_iterator;                   // Const version of row_element_iterator
        typedef value_type*
            col_element_iterator;                         // Iterator to iterate through the elements of a columns of the matrix
        typedef value_type const*
            const_col_element_iterator;                   // Const version of col_element_iterator       
        typedef matrix_element_iterator<matrix,value_type>
            element_iterator;                             // Iterator to iterate through all elements of the matrix (REALLY SLOW! USE row_-/column_iterators INSTEAD!)
        typedef matrix_element_iterator<const matrix,const value_type>
            const_element_iterator;                       // Const version of element_iterator (REALLY SLOW! USE row_-/column_iterators INSTEAD!)

        /**
          * Static function for creating identiy matrix
          *
          */
        static matrix<T,MemoryBlock> identity_matrix(size_type size);

        /**
          * The constructor
          * @param rows the number of rows
          * @param columns the number of columns
          * @param init_value all matrix elements will be initialized to this value.
          */
        explicit matrix(size_type rows = 0, size_type cols = 0, T init_value = T());

        /**
          * Create a matrix from several columns
          * @param colums a vector containing the column ranges (column element iterator pairs)
          */
        template <typename ForwardIterator>
        explicit matrix(std::vector<std::pair<ForwardIterator,ForwardIterator> > const& columns);

        /**
          * The copy constructors
          *
          */
        matrix(matrix const& m);

        template <typename OtherMemoryBlock>
        explicit matrix(matrix<T,OtherMemoryBlock> const& m);

        /**
          * Non-throwing swap function
          * @param r matrix object which should be swapped with the matrix (this)
          */
        void swap(matrix & r);

        /**
          * Swaps two dense_matrices
          */
        friend void swap(matrix & x, matrix & y)
        {
            x.swap(y);
        }

        /**
          * Assigns the matrix to matrix rhs
          */
        matrix& operator = (matrix rhs);

        /**
          * Access the element in row i, column j
          * @param i 0<= i <= num_rows()
          * @param j 0<= j <= num_cols()
          * @return A mutable reference to the matrix element at position (i,j).
          */
        inline value_type& operator()(const size_type i, const size_type j);

        /**
          * Access the element in row i, column j
          * @return A constant reference to the matrix element at position (i,j).
          */
        inline value_type const& operator()(const size_type i, const size_type j) const;

        bool operator == (matrix const& rhs) const;

        matrix<T,MemoryBlock>& operator += (matrix const& rhs);

        matrix<T,MemoryBlock>& operator -= (matrix const& rhs);

        template <typename T2>
        matrix<T,MemoryBlock>& operator *= (T2 const& t);

        template <typename T2>
        matrix<T,MemoryBlock>& operator /= (T2 const& t);

        /**
          * Checks if a matrix is empty
          * @return true if the matrix is a 0x0 matrix, false otherwise.
          */
        inline bool empty() const;

        /**
          * @return number of rows of the matrix
          */
        inline size_type num_rows() const;

        /**
          * @return number of columns of the matrix
          */
        inline size_type num_cols() const;

        /**
          * @return The stride for moving to the next element along a row
          */
        inline difference_type stride1() const;

        /**
          * @return The stride for moving to the next element along a column
          */
        inline difference_type stride2() const;

        /** Resize the matrix.
          * Resizes the matrix to size1 rows and size2 columns and initializes
          * the new elements to init_value. It also enlarges the MemoryBlock
          * if needed. If the new size for any dimension is
          * smaller only elements outside the new size will be deleted.
          * If the new size is larger for any dimension the new elements
          * will be initialized by the init_value.
          * All other elements will keep their value.  
          *
          * Exception behaviour:
          * As long as the assignment and copy operation of the T values don't throw an exception,
          * any exception will leave the matrix unchanged.
          * (Assuming the same behaviour of the underlying MemoryBlock. This is true for std::vector.)
          * @param rows new number of rows
          * @param cols new number of columns
          * @param init_value value to which the new elements will be initalized
          */
        void resize(size_type rows, size_type cols, T const & init_value = T());

        /**
          * Reserves memory for anticipated enlargements of the matrix
          * @param rows For how many rows should memory be reserved, value is ignored if it's smaller than the current number of rows
          * @param cols For how many columns should memory be reserved, value is ignored if it's smaller than the current number of columns
          * @param init_value i
          */
        void reserve(size_type rows, size_type cols, T const & init_value = T());

        std::pair<size_type,size_type> capacity() const;

        bool is_shrinkable() const;

        void clear();

        std::pair<row_element_iterator,row_element_iterator> row(size_type row = 0)
        {
            assert(row < size1_);
            return std::make_pair( row_element_iterator(&values_[row],reserved_size1_), row_element_iterator(&values_[row+reserved_size1_*size2_], reserved_size1_) );
        }

        std::pair<const_row_element_iterator,const_row_element_iterator> row(size_type row = 0) const
        {
            assert(row < size1_);
            return std::make_pair( const_row_element_iterator(&values_[row],reserved_size1_), const_row_element_iterator(&values_[row+reserved_size1_*size2_], reserved_size1_) );
        }
        std::pair<col_element_iterator,col_element_iterator> col(size_type col = 0 )
        {
            assert(col < size2_);
            return std::make_pair( col_element_iterator(&values_[col*reserved_size1_]), col_element_iterator(&values_[col*reserved_size1_+size1_]) );
        }
        std::pair<const_col_element_iterator,const_col_element_iterator> col(size_type col = 0 ) const
        {
            assert(col < size2_);
            return std::make_pair( const_col_element_iterator(&values_[col*reserved_size1_]), const_col_element_iterator(&values_[col*reserved_size1_+size1_]) );
        }
        std::pair<element_iterator,element_iterator> elements()
        {
            return std::make_pair( element_iterator(this,0,0), element_iterator(this,0, num_cols()) );
        }
        std::pair<element_iterator,element_iterator> elements() const
        {
            return std::make_pair( const_element_iterator(this,0,0), const_element_iterator(this,0,num_cols() ) );
        }

        template <typename InputIterator>
        void append_cols(std::pair<InputIterator,InputIterator> const& range, difference_type k = 1);

        template <typename InputIterator>
        void append_rows(std::pair<InputIterator,InputIterator> const& range, difference_type k = 1);

        template <typename InputIterator>
        void insert_rows(size_type i, std::pair<InputIterator,InputIterator> const& range, difference_type k = 1);

        template <typename InputIterator>
        void insert_cols(size_type j, std::pair<InputIterator,InputIterator> const& range, difference_type k = 1);

        void remove_rows(size_type i, difference_type k = 1);

        void remove_cols(size_type j, difference_type k = 1);

        void swap_rows(size_type i1, size_type i2);

        void swap_cols(size_type j1, size_type j2);

        void write_xml(oxstream& ox) const;

        MemoryBlock const& get_values() const;
        MemoryBlock & get_values();



    private:
        template <typename OtherT,typename OtherMemoryBlock>
        friend class matrix;

        template <typename OtherMemoryBlock>
        MemoryBlock copy_values(matrix<T,OtherMemoryBlock> const& m);

        inline bool automatic_reserve(size_type size1, size_type size2, T const& init_value = T());

        size_type size1_;
        size_type size2_;
        size_type reserved_size1_;
        // "reserved_size2_" is done automatically by underlying std::vector (see vector.reserve(), vector.capacity() )
        MemoryBlock values_;
    };

    }  // namespace numeric 
} // namespace alps 

//
// Function hooks
//
namespace alps {
    namespace numeric {

    template<typename T, typename MemoryBlock, typename T2, typename MemoryBlock2>
    typename matrix_vector_multiplies_return_type<matrix<T,MemoryBlock>,vector<T2,MemoryBlock2> >::type
    matrix_vector_multiply(matrix<T,MemoryBlock> const& m, vector<T2,MemoryBlock2> const& v);

    template <typename T,typename MemoryBlock>
    void plus_assign(matrix<T,MemoryBlock>& m, matrix<T,MemoryBlock> const& rhs);

    template <typename T, typename MemoryBlock>
    void minus_assign(matrix<T,MemoryBlock>& m, matrix<T,MemoryBlock> const& rhs);

    template <typename T, typename MemoryBlock, typename T2>
    void multiplies_assign(matrix<T,MemoryBlock>& m, T2 const& t);

    }
}

//
// Free dense matrix functions
//
namespace alps {
    namespace numeric {

    template <typename T, typename MemoryBlock>
    const matrix<T,MemoryBlock> operator + (matrix<T,MemoryBlock> a, matrix<T,MemoryBlock> const& b);

    template <typename T, typename MemoryBlock>
    const matrix<T,MemoryBlock> operator - (matrix<T,MemoryBlock> a, matrix<T,MemoryBlock> const& b);

    template <typename T, typename MemoryBlock>
    const matrix<T,MemoryBlock> operator - (matrix<T,MemoryBlock> a);

    template<typename T, typename MemoryBlock, typename T2, typename MemoryBlock2>
    typename matrix_vector_multiplies_return_type<matrix<T,MemoryBlock>,vector<T2,MemoryBlock2> >::type
    operator * (matrix<T,MemoryBlock> const& m, vector<T2,MemoryBlock2> const& v);

    // TODO: adj(Vector) * Matrix, where adj is a proxy object

    template<typename T,typename MemoryBlock, typename T2>
    typename boost::enable_if<is_matrix_scalar_multiplication<matrix<T,MemoryBlock>,T2>, matrix<T,MemoryBlock> >::type operator * (matrix<T,MemoryBlock> m, T2 const& t);

    template<typename T,typename MemoryBlock, typename T2>
    typename boost::enable_if<is_matrix_scalar_multiplication<matrix<T,MemoryBlock>,T2>, matrix<T,MemoryBlock> >::type operator * (T2 const& t, matrix<T,MemoryBlock> m);

    template<typename T, typename MemoryBlock>
    const matrix<T,MemoryBlock> operator * (matrix<T,MemoryBlock> const& m1, matrix<T,MemoryBlock> const& m2);

    template<class T, class MemoryBlock>
    std::size_t size_of(matrix<T, MemoryBlock> const & m);

    template <typename T, typename MemoryBlock>
    std::ostream& operator << (std::ostream& o, matrix<T,MemoryBlock> const& m);

    template <typename T, typename MemoryBlock>
    alps::oxstream& operator<<(alps::oxstream& xml, matrix<T,MemoryBlock> const& m);

    } // namespace numeric
} // namespace alps


//
// Trait specializations
//
namespace alps {
    namespace numeric {

    //
    // Forward declarations
    //
    template <typename T>
    class diagonal_matrix;

    template <typename T, typename MemoryBlock>
    class vector;

    template<typename T, typename MemoryBlock>
    struct associated_real_vector<matrix<T, MemoryBlock> >
    {
        typedef std::vector<typename real_type<T>::type> type;
    };

    template<class T, class MemoryBlock>
    struct associated_vector<matrix<T,MemoryBlock> >
    {
        typedef std::vector<T> type;
    };

    template<typename T, typename MemoryBlock>
    struct associated_diagonal_matrix<matrix<T, MemoryBlock> >
    {
        typedef  diagonal_matrix<T> type;
    };

    template<typename T, typename MemoryBlock>
    struct associated_real_diagonal_matrix<matrix<T, MemoryBlock> >
    {
        typedef diagonal_matrix<typename real_type<T>::type> type;
    };

    template <typename T1, typename MemoryBlock1, typename T2, typename MemoryBlock2>
    struct matrix_vector_multiplies_return_type<matrix<T1,MemoryBlock1>,vector<T2,MemoryBlock2> >
    {
        private:
            typedef char one;
            typedef long unsigned int two;
            static one test(T1 t) {return one();}
            static two test(T2 t) {return two();}
            typedef typename boost::mpl::if_<typename boost::mpl::bool_<(sizeof(test(T1()*T2())) == sizeof(one))>,T1,T2>::type value_type;
            typedef typename boost::mpl::if_<typename boost::mpl::bool_<(sizeof(test(T1()*T2())) == sizeof(one))>,MemoryBlock1,MemoryBlock2>::type memoryblock_type;
        public:
            typedef alps::numeric::vector<value_type,memoryblock_type> type;
    };

    template <typename T,typename MemoryBlock1, typename MemoryBlock2>
    struct matrix_vector_multiplies_return_type<matrix<T,MemoryBlock1>,vector<T,MemoryBlock2> >
    {
        typedef alps::numeric::vector<T,MemoryBlock2> type;
    };

    }
}


//
// Implement the default matrix interface
//
#define COMMA ,
namespace alps {
namespace numeric {
// If someone has a better idea how to handle the comma, please improve these lines
ALPS_IMPLEMENT_MATRIX_INTERFACE(matrix<T COMMA MemoryBlock>,<typename T COMMA typename MemoryBlock>)
ALPS_IMPLEMENT_MATRIX_ELEMENT_ITERATOR_INTERFACE(matrix<T COMMA MemoryBlock>, <typename T COMMA typename MemoryBlock>)
} // end namespace numeric
} // end namespace alps 
#undef COMMA


#include <alps/numeric/matrix/matrix.ipp>
#endif //ALPS_MATRIX_HPP