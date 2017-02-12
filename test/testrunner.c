
#include "unity_fixture.h"

static void RunAllTests(void)
{
  RUN_TEST_GROUP(led_driver);
  RUN_TEST_GROUP(morse);
}

int main(int argc, const char* argv[]) {
  return UnityMain(argc, argv, RunAllTests);
}
