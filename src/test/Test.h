/*
 * Test.h
 *
 *  Created on: Jul 8, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef TEST_H_
#define TEST_H_

/**
 * The following macro is used to define that a section should be private, except if
 * we are compiling a test.
 */
#ifdef _TEST
#define private public
#endif

/**
 * The following macro is used to define that a section should be protected, except if
 * we are compiling a test.
 */
#ifdef _TEST
#define protected public
#endif

#endif /* TEST_H_ */
