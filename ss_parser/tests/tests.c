/* CTEST_MAIN can be defined only in one of the linked files. Since these are
 * functions defined in a .h -- which eventually gets copied into the .c that
 * includes it --, defining CTEST_MAIN in more than one .c will cause a
 * "multiple definition of function ###" error. */
#define CTEST_MAIN
#include "common.h"


/* Includes headers of other test suites */
#include "bad_header.h"
#include "test_fields.h"
#include "test_inject.h"
#include "test_tsaheylu.h"



int main (int argc, const char* argv[])
{
	return ctest_main(argc, argv);
}

/* How are we defining test suites in this program?
 * ------------------------------------------------
 *
 * [Maybe this infrastructure is not perfectly suited for bigger projects.
 * Still, I think it is good enough for our purposes.]
 *
 * For simple suites, like the `Suite 1`, just write everything here below.
 *
 * For more complex suites, create a .h file the ones included in the beginning
 * of this file. It should contain a CTEST_DATA() (if needed) with the
 * declaration of the data needed, and also a function. The function receives
 * some index, as well as an element of the type defined by CTEST_DATA().
 * Preferrably, this functions should be the only one visible in that file.
 *
 * The implementation of that function should go in a homonimous .c file,
 * In general, the already existing ones simply call a `sanity_checks()`
 * function and, if the test doesn't fail, call a function pointer according
 * according to the index. It is this function pointer that is the actual test.
 * [see the files for a better understanding of how it should work]
 *
 * Finally, for each function pointer (i.e., index), there should be a CTEST2()
 * macro call here below. Of course, if needed, also include CTEST_SETUP() and
 * CTEST_TEARDOWN(), which will run before and after each test respectively.
 *
 *
 * ABOUT HOW `ctest` FINDS THE DEFINITIONS DONE WITH THESE MACROS
 * --------------------------------------------------------------
 *
 * CTest uses a GCC specific feature to find all variables. Each one of the
 * tests is defined with the __attribute__ "section" set to ".ctest". This
 * guarantees that all tests will be placed more or less in the same are of the
 * code.
 *
 * Unfortunately, because of the way it is done, I couldn't not find a way to
 * avoid the listing of each single test to be done. This file, then, has only
 * the main function and a huge list of tests. */



/********************************************************
*                     TESTS FSM PARSER
*********************************************************/

/* Suite 1: pass NULL to parsing main */
CTEST_DATA(fsm_parser_check_NULL) {
	struct ss_data* parsed_data;
};

CTEST2(fsm_parser_check_NULL, test1) {
	data->parsed_data = parsing_main(NULL, TYPE_PERIODIC);

	ASSERT_NULL(data->parsed_data);
}

CTEST2(fsm_parser_check_NULL, test2) {
	data->parsed_data = parsing_main(NULL, TYPE_FIRM_APERIODIC);

	ASSERT_NULL(data->parsed_data);
}

CTEST2(fsm_parser_check_NULL, test3) {
	data->parsed_data = parsing_main(NULL, TYPE_SOFT_APERIODIC);

	ASSERT_NULL(data->parsed_data);
}

CTEST2(fsm_parser_check_NULL, test4) {
	data->parsed_data = parsing_main(NULL, TYPE_END);

	ASSERT_NULL(data->parsed_data);
}


/* Suite 2: passes a file that is right, only with some header problem */
CTEST2(fsm_parser_bad_header, t00) {
	check_bad_header(data, 0, 0);
}

CTEST2(fsm_parser_bad_header, t01) {
	check_bad_header(data, 0, 1);
}

CTEST2(fsm_parser_bad_header, t02) {
	check_bad_header(data, 0, 2);
}

CTEST2(fsm_parser_bad_header, t03) {
	check_bad_header(data, 0, 3);
}

CTEST2(fsm_parser_bad_header, t04) {
	check_bad_header(data, 0, 4);
}

CTEST2(fsm_parser_bad_header, t05) {
	check_bad_header(data, 0, 5);
}

CTEST2(fsm_parser_bad_header, t10) {
	check_bad_header(data, 1, 0);
}

CTEST2(fsm_parser_bad_header, t11) {
	check_bad_header(data, 1, 1);
}

CTEST2(fsm_parser_bad_header, t12) {
	check_bad_header(data, 1, 2);
}

CTEST2(fsm_parser_bad_header, t13) {
	check_bad_header(data, 1, 3);
}

CTEST2(fsm_parser_bad_header, t14) {
	check_bad_header(data, 1, 4);
}

CTEST2(fsm_parser_bad_header, t15) {
	check_bad_header(data, 1, 5);
}

CTEST2(fsm_parser_bad_header, t20) {
	check_bad_header(data, 2, 0);
}

CTEST2(fsm_parser_bad_header, t21) {
	check_bad_header(data, 2, 1);
}

CTEST2(fsm_parser_bad_header, t22) {
	check_bad_header(data, 2, 2);
}

CTEST2(fsm_parser_bad_header, t23) {
	check_bad_header(data, 2, 3);
}

CTEST2(fsm_parser_bad_header, t24) {
	check_bad_header(data, 2, 4);
}

CTEST2(fsm_parser_bad_header, t25) {
	check_bad_header(data, 2, 5);
}

CTEST2(fsm_parser_bad_header, t30) {
	check_bad_header(data, 3, 0);
}

CTEST2(fsm_parser_bad_header, t31) {
	check_bad_header(data, 3, 1);
}

CTEST2(fsm_parser_bad_header, t32) {
	check_bad_header(data, 3, 2);
}

CTEST2(fsm_parser_bad_header, t33) {
	check_bad_header(data, 3, 3);
}

CTEST2(fsm_parser_bad_header, t34) {
	check_bad_header(data, 3, 4);
}

CTEST2(fsm_parser_bad_header, t35) {
	check_bad_header(data, 3, 5);
}

CTEST2(fsm_parser_bad_header, t40) {
	check_bad_header(data, 4, 0);
}

/* The parser is robust enough not to care about those last "\n\r" in the file.
 * That is why we skip this test. Remove the SKIP in the future if needed. */
CTEST2_SKIP(fsm_parser_bad_header, t41) {
	check_bad_header(data, 4, 1);
}

CTEST2(fsm_parser_bad_header, t42) {
	check_bad_header(data, 4, 2);
}

CTEST2(fsm_parser_bad_header, t43) {
	check_bad_header(data, 4, 3);
}

CTEST2(fsm_parser_bad_header, t44) {
	check_bad_header(data, 4, 4);
}

CTEST2(fsm_parser_bad_header, t45) {
	check_bad_header(data, 4, 5);
}
// */

/* Suite 3: passes a file that is right and check the results */
CTEST2(fsm_parser_test_fields, t000) {
	check_test_fields(data, 0, 0, 0);
}

CTEST2(fsm_parser_test_fields, t000100) {
	check_test_fields(data, 0, 1, 0);
}

CTEST2(fsm_parser_test_fields, t000200) {
	check_test_fields(data, 0, 2, 0);
}

CTEST2(fsm_parser_test_fields, t000300) {
	check_test_fields(data, 0, 3, 0);
}

CTEST2(fsm_parser_test_fields, t000400) {
	check_test_fields(data, 0, 4, 0);
}

CTEST2(fsm_parser_test_fields, t000500) {
	check_test_fields(data, 0, 5, 0);
}

CTEST2(fsm_parser_test_fields, t000600) {
	check_test_fields(data, 0, 6, 0);
}

CTEST2(fsm_parser_test_fields, t000700) {
	check_test_fields(data, 0, 7, 0);
}

CTEST2(fsm_parser_test_fields, t000800) {
	check_test_fields(data, 0, 8, 0);
}

CTEST2(fsm_parser_test_fields, t000900) {
	check_test_fields(data, 0, 9, 0);
}

CTEST2(fsm_parser_test_fields, t001000) {
	check_test_fields(data, 0, 10, 0);
}

CTEST2(fsm_parser_test_fields, t001100) {
	check_test_fields(data, 0, 11, 0);
}

CTEST2(fsm_parser_test_fields, t001200) {
	check_test_fields(data, 0, 12, 0);
}

CTEST2(fsm_parser_test_fields, t001300) {
	check_test_fields(data, 0, 13, 0);
}

CTEST2(fsm_parser_test_fields, t001400) {
	check_test_fields(data, 0, 14, 0);
}

CTEST2(fsm_parser_test_fields, t001500) {
	check_test_fields(data, 0, 15, 0);
}

CTEST2(fsm_parser_test_fields, t001600) {
	check_test_fields(data, 0, 16, 0);
}

CTEST2(fsm_parser_test_fields, t001700) {
	check_test_fields(data, 0, 17, 0);
}

CTEST2(fsm_parser_test_fields, t001800) {
	check_test_fields(data, 0, 18, 0);
}


CTEST2(fsm_parser_test_fields, t010000) {
	check_test_fields(data, 1, 0, 0);
}

CTEST2(fsm_parser_test_fields, t010100) {
	check_test_fields(data, 1, 1, 0);
}

CTEST2(fsm_parser_test_fields, t010200) {
	check_test_fields(data, 1, 2, 0);
}

CTEST2(fsm_parser_test_fields, t010300) {
	check_test_fields(data, 1, 3, 0);
}

CTEST2(fsm_parser_test_fields, t010400) {
	check_test_fields(data, 1, 4, 0);
}

CTEST2(fsm_parser_test_fields, t010500) {
	check_test_fields(data, 1, 5, 0);
}

CTEST2(fsm_parser_test_fields, t010600) {
	check_test_fields(data, 1, 6, 0);
}

CTEST2(fsm_parser_test_fields, t010700) {
	check_test_fields(data, 1, 7, 0);
}

CTEST2(fsm_parser_test_fields, t010800) {
	check_test_fields(data, 1, 8, 0);
}

CTEST2(fsm_parser_test_fields, t010900) {
	check_test_fields(data, 1, 9, 0);
}

CTEST2(fsm_parser_test_fields, t011000) {
	check_test_fields(data, 1, 10, 0);
}

CTEST2(fsm_parser_test_fields, t011100) {
	check_test_fields(data, 1, 11, 0);
}

CTEST2(fsm_parser_test_fields, t011200) {
	check_test_fields(data, 1, 12, 0);
}

CTEST2(fsm_parser_test_fields, t011300) {
	check_test_fields(data, 1, 13, 0);
}

CTEST2(fsm_parser_test_fields, t011400) {
	check_test_fields(data, 1, 14, 0);
}

CTEST2(fsm_parser_test_fields, t011500) {
	check_test_fields(data, 1, 15, 0);
}

CTEST2(fsm_parser_test_fields, t011600) {
	check_test_fields(data, 1, 16, 0);
}

CTEST2(fsm_parser_test_fields, t011700) {
	check_test_fields(data, 1, 17, 0);
}

CTEST2(fsm_parser_test_fields, t011800) {
	check_test_fields(data, 1, 18, 0);
}


CTEST2(fsm_parser_test_fields, t020000) {
	check_test_fields(data, 2, 0, 0);
}

CTEST2(fsm_parser_test_fields, t020100) {
	check_test_fields(data, 2, 1, 0);
}

CTEST2(fsm_parser_test_fields, t020200) {
	check_test_fields(data, 2, 2, 0);
}

CTEST2(fsm_parser_test_fields, t020300) {
	check_test_fields(data, 2, 3, 0);
}

CTEST2(fsm_parser_test_fields, t020400) {
	check_test_fields(data, 2, 4, 0);
}

CTEST2(fsm_parser_test_fields, t020500) {
	check_test_fields(data, 2, 5, 0);
}

CTEST2(fsm_parser_test_fields, t020600) {
	check_test_fields(data, 2, 6, 0);
}

CTEST2(fsm_parser_test_fields, t020700) {
	check_test_fields(data, 2, 7, 0);
}

CTEST2(fsm_parser_test_fields, t020800) {
	check_test_fields(data, 2, 8, 0);
}

CTEST2(fsm_parser_test_fields, t020900) {
	check_test_fields(data, 2, 9, 0);
}

CTEST2(fsm_parser_test_fields, t021000) {
	check_test_fields(data, 2, 10, 0);
}

CTEST2(fsm_parser_test_fields, t021100) {
	check_test_fields(data, 2, 11, 0);
}

CTEST2(fsm_parser_test_fields, t021200) {
	check_test_fields(data, 2, 12, 0);
}

CTEST2(fsm_parser_test_fields, t021300) {
	check_test_fields(data, 2, 13, 0);
}

CTEST2(fsm_parser_test_fields, t021400) {
	check_test_fields(data, 2, 14, 0);
}

CTEST2(fsm_parser_test_fields, t021500) {
	check_test_fields(data, 2, 15, 0);
}

CTEST2(fsm_parser_test_fields, t021600) {
	check_test_fields(data, 2, 16, 0);
}

CTEST2(fsm_parser_test_fields, t021700) {
	check_test_fields(data, 2, 17, 0);
}

CTEST2(fsm_parser_test_fields, t021800) {
	check_test_fields(data, 2, 18, 0);
}

// */





/********************************************************
*                     TESTS SS_INJECT
*********************************************************/
/* Just modularizing the "interfacing" functions of ss_inject and ss_tsaheylu
 * is not enough. I should follow the same "function pointer ideas" to
 * "modularize" the internal functions. When this is done, I can replace those
 * functions with my own testing functions and check if the values with which
 * they were called are right. */

/* Things to test:
 * 1) Use inj->set_res_id() to set an id to the reservation.
 *    Use get_res_id() to guarantee it is getting the right number.
 *
 * 2) Use inj->set_res_id() to set an id to the reservation.
 *    Replace the call to "reservation_create()" with a checking function.
 *    Call inj->res_create() with some specific input.
 *
 * 3) In inj->inj_per_task():
 *    Replace the call to "prepare_binary()" and check that the expected values
 *        were passed to it.
 *    Also replace the call to "inject_table()" and check if the right values
 *        were passed to it.
 *     -----------> maybe even "inject_table()" should be part of the interface.
 *                  Then ss_inject_task_table() should be a "hidden" function.
 *
 * 4) In inj->inj_interval():
 *    Replace the call to slot_shift_add_interval() to a checking function.
 *    Check values passed to it.
 *
 *
 * ?) How to test "wait_till_release()" ?
 */

CTEST_SETUP(test_inject) {
	data->inj = malloc(sizeof(struct ss_inject));
	ss_inject_init(data->inj);
}

CTEST_TEARDOWN(test_inject) {
	ss_inject_del(data->inj);
}

CTEST2(test_inject, t0) {
	check_test_inject(data, 0);
}

CTEST2(test_inject, t1) {
	check_test_inject(data, 1);
}

CTEST2(test_inject, t2) {
	check_test_inject(data, 2);
}

CTEST2(test_inject, t3) {
	check_test_inject(data, 3);
}

CTEST2(test_inject, t4) {
	check_test_inject(data, 4);
}

CTEST2(test_inject, t5) {
	check_test_inject(data, 5);
}

CTEST2(test_inject, t6) {
	check_test_inject(data, 6);
}


// */

/********************************************************
*                     TESTS SS_TSAHEYLU
*********************************************************/
CTEST_SETUP(test_tsaheylu) {
	data->tsa = malloc(sizeof(struct ss_tsaheylu));
	data->tsa->inj = NULL;
	ss_tsaheylu_init(data->tsa, NULL);
}

CTEST_TEARDOWN(test_tsaheylu) {
	ss_tsaheylu_del(data->tsa);
}

CTEST2(test_tsaheylu, t0) {
	check_test_tsaheylu(data, 0);
}

CTEST2(test_tsaheylu, t1) {
	check_test_tsaheylu(data, 1);
}





