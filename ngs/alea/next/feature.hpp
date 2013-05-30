/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                 *
 * ALPS Project: Algorithms and Libraries for Physics Simulations                  *
 *                                                                                 *
 * ALPS Libraries                                                                  *
 *                                                                                 *
 * Copyright (C) 2011 - 2013 by Mario Koenz <mkoenz@ethz.ch>                       *
 *                              Lukas Gamper <gamperl@gmail.com>                   *
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

#ifndef ALPS_NGS_ACCUMULATOR_FEATURE_HPP
#define ALPS_NGS_ACCUMULATOR_FEATURE_HPP

#include <boost/utility.hpp>

namespace alps {
	namespace accumulator {

		template<typename T, typename F> struct has_feature 
			: public boost::false_type
		{};

		template<typename T> struct has_result_type {
	        template<typename U> static char check(typename U::result_type *);
	        template<typename U> static double check(...);
			typedef boost::integral_constant<bool, sizeof(char) == sizeof(check<T>(0))> type;
	    };

		template<typename T> struct value_type {
	    	typedef typename T::value_type type;
		};

		namespace impl {

			template<typename T> struct ResultBase {
				typedef T value_type;
			};

			template<typename T> struct AccumulatorBase {
				typedef T value_type;
				typedef ResultBase<T> result_type; 
			};

			template<typename T, typename F, typename B> struct Accumulator {};

			template<typename T, typename F, typename B> class Result {};

			template<typename F, typename B> class BaseWrapper {};

			template<typename T, typename F, typename B> class ResultTypeWrapper {};

			template<typename A, typename F, typename B> class DerivedWrapper {};

		}
	}
}

 #endif