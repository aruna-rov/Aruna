//
// Created by noeel on 09-03-20.
//

#include <catch2/catch.hpp>
#include <aruna.h>

using namespace aruna::log;

int new_print(const char *format, va_list v) {
    REQUIRE(strlen(format) > 1);
//    magic number!
    return 9876;
}

SCENARIO("logging", "[log]") {
    GIVEN("create log channel") {
        char channel_name[] = "test_channel";
        level_t log_level = level_t::INFO;
        channel_t log = channel_t(channel_name, log_level);
        THEN("startup should have no errors") {
            REQUIRE(log.startup_status == aruna::err_t::OK);
        }

        WHEN("create identical log channel") {
            channel_t log2 = channel_t(channel_name);
            THEN("an error must accour for log2") {
                REQUIRE(log2.startup_status == aruna::err_t::CHANNEL_EXISTS);
            }
            WHEN("Printing") {
                int p = 0;
                p = log2.error("error print");
                THEN("log 2 cant print anything") {
                    CHECK(p > 0); // TODO
                }
            }
        }
        THEN("name should be stored") {
            REQUIRE_FALSE(strcmp(log.name, channel_name));
        }
        THEN("level should be set") {
            REQUIRE(log.level == log_level);
        }

        WHEN("printing debug message") {
            int lines_printed = log.debug("debug message");
            THEN("won't be printed") {
                REQUIRE(lines_printed == 0);
            }
        }
        WHEN("printing verbose message") {
            int lines_printed = log.verbose("verbose message");
            THEN("won't be printed") {
                REQUIRE(lines_printed == 0);
            }
        }
        WHEN("printing info message") {
            int lines_printed = log.info("info message");
            THEN("be printed") {
                REQUIRE(lines_printed > 1);
            }
        }
        WHEN("printing warning message") {
            int lines_printed = log.warning("warning message");
            THEN("be printed") {
                REQUIRE(lines_printed > 1);
            }
        }
        WHEN("printing error message") {
            int lines_printed = log.error("error message");
            THEN("be printed") {
                REQUIRE(lines_printed > 1);
            }
        }

        WHEN("printing formatted message") {
            char s[] = "my_format";
            float f = 1.23;
            int i = 456;
            int lines_printed = log.info("s:%s, i:%i, x:0x%x, f:%f\n", s, i, i, f);
            THEN ("characters are printed") {
                REQUIRE(lines_printed == 74);
            }
        }

        WHEN("changing printf") {
            vprintf_like_t old_func;
            old_func = set_print_function(new_print);
            AND_WHEN("printing stuff") {
                int l = 0;
                l = log.info("new print?");
                THEN("printed using new_print") {
//                    magic number!
                    REQUIRE(l == 9876);
                }
            }
//            cleaning up, because for some reason Catch does not?
            set_print_function(old_func);
        }
        WHEN("lowering log level") {
            bool success = set_level(channel_name, level_t::VERBOSE);
            THEN("should succeed") {
                REQUIRE(success);
            }
            AND_WHEN("printing verbose") {
                int l = log.verbose("verbose");
                THEN("printed") {
                    REQUIRE(l > 1);
                }
            }
        }
        WHEN("highering log level") {
            set_level(channel_name, level_t::NONE);
            AND_WHEN("printing error") {
                int l = log.error("error");
                THEN("not printed") {
                    REQUIRE(l == 0);
                }
            }
        }

        WHEN("dumping data") {
            uint8_t d[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef};
            int l = log.dump(level_t::INFO, d, 8);
            THEN ("wrote 8 bytes") {
                REQUIRE(l == 8);
            }
        }
        WHEN("dumping with level NONE data") {
            uint8_t d[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef};
            int l = log.dump(level_t::NONE, d, 8);
            THEN ("wrote 0 bytes") {
                REQUIRE(l == 0);
            }
        }
        WHEN("dumping with invalid level data") {
            uint8_t d[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef};
            int l = log.dump((level_t) -99, d, 8);
            THEN ("wrote 8 bytes") {
                REQUIRE(l == 8);
            }
        }
        WHEN("changing max level to error") {
            level_t old_max = set_max_level(level_t::ERROR);
            AND_WHEN("printing warning") {
                int l = log.warning("warn");
                THEN("not printed") {
                    REQUIRE(l == 0);
                }
            }
            AND_WHEN("printing error") {
                int l = log.error("error");
                THEN("printed") {
                    REQUIRE(l > 1);
                }
            }
        }
        WHEN("changing max level to impossible") {
            level_t old_max = set_max_level((level_t) -23);
            AND_WHEN("printing warning") {
                int l = log.warning("warn");
                THEN("not printed") {
                    REQUIRE(l == 0);
                }
            }
            AND_WHEN("printing error") {
                int l = log.error("error");
                THEN("not printed") {
                    REQUIRE(l == 0);
                }
            }
        }

    }
}

SCENARIO("convert level_t to char", "[log]") {
    WHEN("convert level_t::NONE to char") {
        char buf[5];
        level_t_to_char(level_t::NONE, buf);
        THEN("buffer is `NONE`") {
            REQUIRE_FALSE(strcmp("NONE ", buf));
        }
    }
    WHEN("convert level_t::VERBOSE to char") {
        char buf[5];
        level_t_to_char(level_t::VERBOSE, buf);
        THEN("buffer is `VERBO`") {
            REQUIRE_FALSE(strcmp("VERBO", buf));
        }
    }
    WHEN("convert level_t::DEBUG to char") {
        char buf[5];
        level_t_to_char(level_t::DEBUG, buf);
        THEN("buffer is `DEBUG`") {
            REQUIRE_FALSE(strcmp("DEBUG", buf));
        }
    }
    WHEN("convert level_t::INFO to char") {
        char buf[5];
        level_t_to_char(level_t::INFO, buf);
        THEN("buffer is `INFO `") {
            REQUIRE_FALSE(strcmp("INFO ", buf));
        }
    }
    WHEN("convert level_t::WARNING to char") {
        char buf[5];
        level_t_to_char(level_t::WARNING, buf);
        THEN("buffer is `WARN `") {
            REQUIRE_FALSE(strcmp("WARN ", buf));
        }
    }
    WHEN("convert level_t::ERROR to char") {
        char buf[5];
        level_t_to_char(level_t::ERROR, buf);
        THEN("buffer is `ERROR`") {
            REQUIRE_FALSE(strcmp("ERROR", buf));
        }
    }
    WHEN("convert invalid to char") {
        char buf[5];
        level_t_to_char((level_t) -100, buf);
        THEN("buffer is `lvler`") {
            REQUIRE_FALSE(strcmp("lvler", buf));
        }
    }
}