/* Copyright (c) AIRBUS and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <exception>
#include <list>

#include "pegtl.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "parser.hh"
#include "utils/pegtl_spdlog_tracer.hh"

#include "parser_state.hh"
#include "parser_action.hh"
#include "parser_skip.hh"
#include "parse_name.hh"
#include "parse_domain.hh"
#include "parse_problem.hh"

namespace pegtl = TAO_PEGTL_NAMESPACE;  // NOLINT

namespace skdecide {

    namespace pddl {

        namespace parser {

            // parse domain

            struct domain : pegtl::if_must<
                                pegtl::seq<
                                    pegtl::one<'('>,
                                    ignored,
                                    keyword<'d', 'e', 'f', 'i', 'n', 'e'>,
                                    ignored,
                                    domain_name,
                                    ignored
                                >,
                                pegtl::seq<
                                    preamble,
                                    ignored,
                                    pegtl::one<')'>
                                >
                            > {};
            
            template <>
            struct action<domain> {
                static void apply0(state& s) {
                    // release current domain
                    s.domain.reset();
                    s.registered_objects.clear();
                }
            };

            struct problem : pegtl::if_must<
                                pegtl::seq<
                                    pegtl::one<'('>,
                                    ignored,
                                    keyword<'d', 'e', 'f', 'i', 'n', 'e'>,
                                    ignored,
                                    problem_name,
                                    ignored,
                                    problem_domain_name,
                                    ignored
                                >,
                                pegtl::seq<
                                    problem_body,
                                    ignored,
                                    pegtl::one<')'>
                                >
                             > {};
            
            template <>
            struct action<problem> {
                static void apply0(state& s) {
                    // release current problem
                    s.problem.reset();
                    s.registered_objects.clear();
                }
            };

            struct grammar : pegtl::star<pegtl::seq<ignored, pegtl::sor<domain, problem>, ignored>> {};

        } // namespace parser

        void Parser::parse(const std::list<std::string>& files,
                           std::list<Domain::Ptr>& domains,
                           std::list<Problem::Ptr>& problems,
                           bool debug_logs) {
            if (debug_logs) {
                spdlog::set_level(spdlog::level::debug);
            } else {
                spdlog::set_level(spdlog::level::info);
            }

            parser::state s;

            // Load known domains
            for (auto& d : domains) {
                s.domains.insert(std::make_pair(d->get_name(), d));
            }

            for (const std::string& f : files) {
                spdlog::info("Parsing " + f);
                pegtl::file_input<> in(f);
                
                try {
                    if (debug_logs) {
                        pegtl::trace_state t;
                        if (!pegtl::parse<parser::grammar, parser::action, pegtl::tracer>(in, t, s)) {
                            throw std::runtime_error("SKDECIDE exception: unable to parse " + f);
                        }
                    } else {
                        if (!pegtl::parse<parser::grammar, parser::action>(in, s)) {
                            throw std::runtime_error("SKDECIDE exception: unable to parse " + f);
                        }
                    }
                } catch (const pegtl::parse_error& e) {
                    const auto p = e.positions().front();
                    throw std::runtime_error("SKDECIDE parsing exception: " + std::string(e.what()) + '\n' +
                                             std::string(in.line_at( p )) + '\n' + std::string( p.column, ' ' ) + '^');
                } catch (const std::system_error& e) {
                    throw std::runtime_error("SKDECIDE exception: error when reading " + f + ": " + e.what());
                } catch (const std::exception& e) {
                    throw std::runtime_error("SKDECIDE exception: error when parsing " + f + ": " + e.what());
                }
            }

            for (const auto& d : s.domains) {
                domains.push_back(d.second);
            }

            for (const auto& p : s.problems) {
                problems.push_back(p.second);
            }
        }

    } // namespace pddl

} // namespace skdecide
