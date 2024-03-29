/*! 
    \brief Simple unit testing for C/C++
    \author Cosmin Cremarenco
    \copyright 20212, Cosmin Cremarenco, under Apache 2.0 license
    \link https://github.com/ccosmin/tinytest

    \details
    Purpose: facilitate the unit testing for programs written in c/c++

    Use:
    Define your tests as functions that don't take any arguments
    but return "int". They should return 1 if successful otherwise, 0.
    Then add them to a test suite. Finally,
    if all you want to run are the tests inside the same .c/.cpp translation
    unit add call to TINYTEST_MAIN_SINGLE_SUITE which takes as argument
    the name of the test suite to run.
    Otherwise, if you wish to gather test suites across translation units
    you need to declare your test suites using TINYTEST_DECLARE_SUITE
    and include the .h with the declarations inside a .c/.cpp file
    which will call TINYTEST_START_MAIN/TINYTEST_END_MAIN in which 
    you will put all test suites.

    Please see examples for more details.

    Tests are meant to be in the same place as the tested code.
    Declare TINYTEST_NOTEST if you don't want to compile the test code.
    Usually you will declare TINYTEST_NOTEST together with NDEBUG.

    \brief + with a few modifications.
    \author $Author$
    \version $Id$
 */

#ifndef TINYTEST_H_
#define TINYTEST_H_

#include <stdlib.h>
#include <stdio.h>

/* Additional text formating features
 * see: https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#text-formatting
 */
#ifdef COLORIFY_TERMINAL_OUTPUT
#define COLORIFY_ERROR_BEGIN "\x1b[1m\x1b[31m" /* Set brightness/intensity flag and red color to foreground. */
#else
#define COLORIFY_ERROR_BEGIN "" /* No formating. */
#endif /* COLORIFY_TERMINAL_OUTPUT */

#ifdef COLORIFY_TERMINAL_OUTPUT
#define  COLORIFY_ERROR_END "\x1b[0m" /* Return to default console's text formatting. */
#else
#define COLORIFY_ERROR_END "" /* No formating. */
#endif /* COLORIFY_TERMINAL_OUTPUT */

typedef int (*TinyTestFunc)(const char *);

typedef struct TinyTestStruct
{
  TinyTestFunc m_func;
  const char* m_name;
  TinyTestFunc m_setup;
  TinyTestFunc m_teardown;
  struct TinyTestStruct* m_next;
} TinyTest;

typedef struct TinyTestSuiteStruct
{
  struct TinyTestStruct* m_head;
  const char* m_name;
  struct TinyTestStruct* m_headTest;
  struct TinyTestSuiteStruct* m_next;
} TinyTestSuite;

typedef struct TinyTestRegistryStruct
{
  TinyTestSuite* m_headSuite;
  const char* m_name;
} TinyTestRegistry;


#ifndef TINYTEST_NOTEST

#define TINYTEST_FALSE_MSG(actual, ...)                                 \
  if ( (actual) )                                                       \
  {                                                                     \
    fprintf(                                                            \
            stderr,                                                     \
            COLORIFY_ERROR_BEGIN                                        \
            "ERROR: "                                                   \
            "%s:%d false, actual: %s\n",                                \
            COLORIFY_ERROR_END                                          \
           __FILE__, __LINE__, #actual                                  \
    );                                                                  \
    printf( __VA_ARGS__ );                                              \
    return 0;                                                           \
  }

#define TINYTEST_FALSE(actual)                                          \
  TINYTEST_FALSE_MSG(actual, NULL)

#define TINYTEST_TRUE_MSG(actual, ...)                                  \
  if ( !(actual) )                                                      \
  {                                                                     \
    fprintf(                                                            \
            stderr,                                                     \
            COLORIFY_ERROR_BEGIN                                        \
            "ERROR: "                                                   \
            COLORIFY_ERROR_END                                          \
            "%s:%d true, actual: %s\n",                                 \
           __FILE__, __LINE__, #actual                                  \
    );                                                                  \
    printf( __VA_ARGS__ );                                              \
    return 0;                                                           \
  }

#define TINYTEST_TRUE(actual)                                           \
  TINYTEST_TRUE_MSG(actual, NULL)

#define TINYTEST_EQUAL_MSG(expected, actual, ...)                       \
  if ( (expected) != (actual) )                                         \
  {                                                                     \
    fprintf(                                                            \
            stderr,                                                     \
            COLORIFY_ERROR_BEGIN                                        \
            "ERROR: "                                                   \
            COLORIFY_ERROR_END                                          \
            "%s:%d expected %s, actual: %s\n",                          \
           __FILE__, __LINE__, #expected, #actual);                     \
    printf( __VA_ARGS__ );                                              \
    return 0;                                                           \
  }

#define TINYTEST_EQUAL(expected, actual)                                \
  TINYTEST_EQUAL_MSG(expected, actual, NULL)

#define TINYTEST_STR_NOT_EQUAL_MSG(expected, actual, ...)                   \
  if ( !strcmp((expected), (actual)) )                                   \
  {                                                                     \
    fprintf(                                                            \
            stderr,                                                     \
            COLORIFY_ERROR_BEGIN                                        \
            "ERROR: "                                                   \
            COLORIFY_ERROR_END                                          \
            "%s:%d unexpected \"%s\", actual: \"%s\"\n",                  \
           __FILE__, __LINE__, expected, actual);                       \
    /*if ( NULL != msg ) printf( msg );*/                               \
    printf( __VA_ARGS__ );                                              \
    return 0;                                                           \
  }

#define TINYTEST_STR_NOT_EQUAL(expected, actual)                            \
  TINYTEST_STR_NOT_EQUAL_MSG(expected, actual, NULL)

#define TINYTEST_STR_EQUAL_MSG(expected, actual, ...)                   \
  if ( strcmp((expected), (actual)) )                                   \
  {                                                                     \
    fprintf(                                                            \
            stderr,                                                     \
            COLORIFY_ERROR_BEGIN                                        \
            "ERROR: "                                                   \
            COLORIFY_ERROR_END                                          \
            "%s:%d expected \"%s\", actual: \"%s\"\n",                  \
           __FILE__, __LINE__, expected, actual);                       \
    /*if ( NULL != msg ) printf( msg );*/                               \
    printf( __VA_ARGS__ );                                              \
    return 0;                                                           \
  }

#define TINYTEST_STR_EQUAL(expected, actual)                            \
  TINYTEST_STR_EQUAL_MSG(expected, actual, NULL)

#define TINYTEST_BIN_ARR_EQUAL_MSG(expected, actual, size, ...)         \
  if ( memcmp(expected, actual, size) )                                 \
  {                                                                     \
    fprintf(                                                            \
            stderr,                                                     \
            COLORIFY_ERROR_BEGIN                                        \
            "ERROR: "                                                   \
            COLORIFY_ERROR_END                                          \
            "%s:%d expected \"%s\", actual: \"%s\"\n",                  \
           __FILE__, __LINE__, expected, actual);                       \
    printf( __VA_ARGS__ );                                              \
    return 0;                                                           \
  }

#define TINYTEST_BIN_ARR_EQUAL(expected, actual, size)                  \
  TINYTEST_BIN_ARR_EQUAL_MSG(expected, actual, size, NULL)

#define TINYTEST_ASSERT_MSG(assertion, ...)                             \
  if ( !(assertion) )                                                   \
  {                                                                     \
    fprintf(                                                            \
            stderr,                                                     \
            COLORIFY_ERROR_BEGIN                                        \
            "ERROR: "                                                   \
            COLORIFY_ERROR_END                                          \
            "%s:%d assertion failed: \"%s\"\n",                         \
           __FILE__, __LINE__, #assertion);                             \
    printf( __VA_ARGS__ );                                              \
    return 0;                                                           \
  }

#define TINYTEST_ASSERT(assertion)                                      \
  TINYTEST_ASSERT_MSG(assertion, NULL)

/* cppcheck-suppress misra-c2012-20.10 */
#define TINYTEST_DECLARE_SUITE(suiteName)                               \
  void Suite##suiteName(TinyTestRegistry* registry)

/* cppcheck-suppress misra-c2012-20.10 */
#define TINYTEST_START_SUITE(suiteName)                                 \
void Suite##suiteName(TinyTestRegistry* registry)                       \
{                                                                       \
  TinyTestSuite* suite = (TinyTestSuite*)malloc(sizeof(TinyTestSuite)); \
  suite->m_name = #suiteName;                                           \
  suite->m_headTest = NULL;                                             \
  suite->m_next = NULL;
  
/* cppcheck-suppress misra-c2012-20.10 */
#define TINYTEST_ADD_TEST(test,setup,teardown)                          \
  TinyTest* test##decl = (TinyTest*)malloc(sizeof(TinyTest));           \
  test##decl->m_func = test;                                            \
  test##decl->m_name = #test;                                           \
  test##decl->m_setup = setup;                                          \
  test##decl->m_teardown = teardown;                                    \
  test##decl->m_next = suite->m_headTest;                               \
  suite->m_headTest = test##decl;

#define TINYTEST_END_SUITE()                                            \
  suite->m_next = registry->m_headSuite;                                \
  registry->m_headSuite = suite;                                        \
}

#define TINYTEST_START_MAIN()                                           \
  int main(                                                             \
            int argc __attribute__((unused)),                           \
            char* argv[] __attribute__((unused)) )                      \
  {                                                                     \
    TinyTestRegistry registry;                                          \
    registry.m_headSuite = NULL;

#define TINYTEST_REG_NAME(registryName)                                 \
  registry.m_name = registryName;

/* cppcheck-suppress misra-c2012-20.10 */
#define TINYTEST_RUN_SUITE(suiteName)                                   \
  Suite##suiteName(&registry);

#define TINYTEST_INTERNAL_FREE_TESTS()                                  \
  {                                                                     \
    TinyTestSuite* s = registry.m_headSuite;                            \
    TinyTestSuite* ss = NULL;                                           \
    for ( ; s; s = ss )                                                 \
    {                                                                   \
      ss = s->m_next;                                                   \
      TinyTest* t = s->m_headTest;                                      \
      TinyTest* tt = NULL;                                              \
      for ( ; t; t = tt )                                               \
      {                                                                 \
        tt = t->m_next;                                                 \
        free(t);                                                        \
      }                                                                 \
      free(s);                                                          \
    }                                                                   \
  }

#define TINYTEST_INTERNAL_RUN_TESTS()                                   \
  {                                                                     \
    int iRc = 0;                                                        \
    int okTests = 0;                                                    \
    int failedTests = 0;                                                \
    printf("\nTest of \"%s\"\n---", registry.m_name);                   \
    TinyTestSuite* s = registry.m_headSuite;                            \
    for ( ; s; s = s->m_next )                                          \
    {                                                                   \
      printf("\n\nRun suite: \"\%s\"\n---", s->m_name);                 \
      TinyTest* t = s->m_headTest;                                      \
      for ( ; t; t = t->m_next )                                        \
      {                                                                 \
        if ( t->m_setup )                                               \
        {                                                               \
            printf("\nSetup for \"%s\"\n", t->m_name );                 \
            (*t->m_setup)(t->m_name);                                   \
        }                                                               \
        printf("\nRun \"%s\": ", t->m_name );                           \
        iRc = (*t->m_func)(t->m_name);                                  \
        if ( t->m_teardown )                                            \
        {                                                               \
            printf("\nTeardown for \"%s\"\n", t->m_name );              \
          (*t->m_teardown)(t->m_name);                                  \
        }                                                               \
        if ( iRc )                                                      \
        {                                                               \
          printf("Test result: OK");                                    \
          ++okTests;                                                    \
        }                                                               \
        else                                                            \
        {                                                               \
          printf("Test result: FAIL");                                  \
          ++failedTests;                                                \
        }                                                               \
      }                                                                 \
    }                                                                   \
    printf("\nSummary of tests\n---\n OK : %d", okTests);               \
    if ( failedTests )                                                  \
    {                                                                   \
      printf(                                                           \
              COLORIFY_ERROR_BEGIN                                      \
              "  FAILED : %d"                                           \
              COLORIFY_ERROR_END,                                       \
              failedTests                                               \
      );                                                                \
    }                                                                   \
    printf("\n");                                                       \
  }

#define TINYTEST_END_MAIN()                                             \
    TINYTEST_INTERNAL_RUN_TESTS()                                       \
    printf("\n");                                                       \
    TINYTEST_INTERNAL_FREE_TESTS()                                      \
  }

#define TINYTEST_MAIN_SINGLE_SUITE(suiteName)                           \
  TINYTEST_START_MAIN();                                                \
  TINYTEST_RUN_SUITE(suiteName);                                        \
  TINYTEST_END_MAIN();

#else /* TINYTEST_NOTEST defined */

#define TINYTEST_FALSE_MSG(actual, ...)
#define TINYTEST_FALSE(actual)
#define TINYTEST_TRUE_MSG(actual, ...)
#define TINYTEST_TRUE(actual)
#define TINYTEST_EQUAL_MSG(expected, actual, ...)
#define TINYTEST_EQUAL(expected, actual)
#define TINYTEST_STR_EQUAL_MSG(expected, actual, ...)
#define TINYTEST_STR_EQUAL(expected, actual)
#define TINYTEST_ASSERT_MSG(assertion, ...)
#define TINYTEST_ASSERT(assertion)

#define TINYTEST_START_SUITE(suiteName)
#define TINYTEST_ADD_TEST(test)
#define TINYTEST_END_SUITE()
#define TINYTEST_START_MAIN()
#define TINYTEST_RUN_SUITE(suiteName)
#define TINYTEST_INTERNAL_FREE_TESTS()
#define TINYTEST_INTERNAL_RUN_TESTS()
#define TINYTEST_END_MAIN()
#define TINYTEST_MAIN_SINGLE_SUITE(suiteName)

#endif /* TINYTEST_NOTEST */

#endif /* TINYTEST_H_ */
