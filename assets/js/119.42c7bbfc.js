(window.webpackJsonp=window.webpackJsonp||[]).push([[119],{631:function(e,t,a){"use strict";a.r(t);var s=a(38),r=Object(s.a)({},(function(){var e=this,t=e.$createElement,a=e._self._c||t;return a("ContentSlotsDistributor",{attrs:{"slot-key":e.$parent.slotKey}},[a("h1",{attrs:{id:"hub-solver-pomcp-pomcp"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#hub-solver-pomcp-pomcp"}},[e._v("#")]),e._v(" hub.solver.pomcp.pomcp")]),e._v(" "),a("div",{staticClass:"custom-block tip"},[a("p",{staticClass:"custom-block-title"},[e._v("Domain specification")]),e._v(" "),a("skdecide-summary")],1),e._v(" "),a("h2",{attrs:{id:"pomcp"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#pomcp"}},[e._v("#")]),e._v(" POMCP")]),e._v(" "),a("p",[e._v("Partially-Observable Monte Carlo Planning solver.")]),e._v(" "),a("h3",{attrs:{id:"constructor"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#constructor"}},[e._v("#")]),e._v(" Constructor "),a("Badge",{attrs:{text:"POMCP",type:"tip"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"POMCP",sig:{params:[{name:"domain_factory",annotation:"Callable[[], Domain]"},{name:"max_iterations",default:"5000"},{name:"max_depth",default:"50"},{name:"n_samples",default:"5000"},{name:"callback",default:"<lambda function>",annotation:"Callable[[POMCP], bool]"}],return:"None"}}}),e._v(" "),a("h4",{attrs:{id:"parameters"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters"}},[e._v("#")]),e._v(" Parameters")]),e._v(" "),a("ul",[a("li",[a("strong",[e._v("domain_factory")])]),e._v(" "),a("li",[a("strong",[e._v("max_iterations")])]),e._v(" "),a("li",[a("strong",[e._v("max_depth")])]),e._v(" "),a("li",[a("strong",[e._v("n_samples")])]),e._v(" "),a("li",[a("strong",[e._v("callback")]),e._v(": function called at each solver iteration. If returning true, the solve process stops.")])]),e._v(" "),a("h3",{attrs:{id:"autocast"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#autocast"}},[e._v("#")]),e._v(" autocast "),a("Badge",{attrs:{text:"Solver",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"autocast",sig:{params:[{name:"self"},{name:"domain_cls",default:"None",annotation:"Optional[Type[Domain]]"}],return:"None"}}}),e._v(" "),a("p",[e._v("Autocast itself to the level corresponding to the given domain class.")]),e._v(" "),a("h4",{attrs:{id:"parameters-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-2"}},[e._v("#")]),e._v(" Parameters")]),e._v(" "),a("ul",[a("li",[a("strong",[e._v("domain_cls")]),e._v(": the domain class to which level the solver needs to autocast itself.\nBy default, use the original domain factory passed to its constructor.")])]),e._v(" "),a("h3",{attrs:{id:"check-domain"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#check-domain"}},[e._v("#")]),e._v(" check_domain "),a("Badge",{attrs:{text:"Solver",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"check_domain",sig:{params:[{name:"domain",annotation:"Domain"}],return:"bool"}}}),e._v(" "),a("p",[e._v("Check whether a domain is compliant with this solver type.")]),e._v(" "),a("p",[e._v("By default, "),a("code",[e._v("Solver.check_domain()")]),e._v(" provides some boilerplate code and internally\ncalls "),a("code",[e._v("Solver._check_domain_additional()")]),e._v(' (which returns True by default but can be overridden  to define\nspecific checks in addition to the "domain requirements"). The boilerplate code automatically checks whether all\ndomain requirements are met.')]),e._v(" "),a("h4",{attrs:{id:"parameters-3"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-3"}},[e._v("#")]),e._v(" Parameters")]),e._v(" "),a("ul",[a("li",[a("strong",[e._v("domain")]),e._v(": The domain to check.")])]),e._v(" "),a("h4",{attrs:{id:"returns"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns"}},[e._v("#")]),e._v(" Returns")]),e._v(" "),a("p",[e._v("True if the domain is compliant with the solver type (False otherwise).")]),e._v(" "),a("h3",{attrs:{id:"get-domain-requirements"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#get-domain-requirements"}},[e._v("#")]),e._v(" get_domain_requirements "),a("Badge",{attrs:{text:"Solver",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"get_domain_requirements",sig:{params:[],return:"List[type]"}}}),e._v(" "),a("p",[e._v("Get domain requirements for this solver class to be applicable.")]),e._v(" "),a("p",[e._v("Domain requirements are classes from the "),a("code",[e._v("skdecide.builders.domain")]),e._v(" package that the domain needs to inherit from.")]),e._v(" "),a("h4",{attrs:{id:"returns-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-2"}},[e._v("#")]),e._v(" Returns")]),e._v(" "),a("p",[e._v("A list of classes to inherit from.")]),e._v(" "),a("h3",{attrs:{id:"get-next-action"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#get-next-action"}},[e._v("#")]),e._v(" get_next_action "),a("Badge",{attrs:{text:"DeterministicPolicies",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"get_next_action",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"D.T_agent[D.T_concurrency[D.T_event]]"}}}),e._v(" "),a("p",[e._v("Get the next deterministic action (from the solver's current policy).")]),e._v(" "),a("h4",{attrs:{id:"parameters-4"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-4"}},[e._v("#")]),e._v(" Parameters")]),e._v(" "),a("ul",[a("li",[a("strong",[e._v("observation")]),e._v(": The observation for which next action is requested.")])]),e._v(" "),a("h4",{attrs:{id:"returns-3"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-3"}},[e._v("#")]),e._v(" Returns")]),e._v(" "),a("p",[e._v("The next deterministic action.")]),e._v(" "),a("h3",{attrs:{id:"get-next-action-distribution"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#get-next-action-distribution"}},[e._v("#")]),e._v(" get_next_action_distribution "),a("Badge",{attrs:{text:"UncertainPolicies",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"get_next_action_distribution",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"Distribution[D.T_agent[D.T_concurrency[D.T_event]]]"}}}),e._v(" "),a("p",[e._v("Get the probabilistic distribution of next action for the given observation (from the solver's current\npolicy).")]),e._v(" "),a("h4",{attrs:{id:"parameters-5"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-5"}},[e._v("#")]),e._v(" Parameters")]),e._v(" "),a("ul",[a("li",[a("strong",[e._v("observation")]),e._v(": The observation to consider.")])]),e._v(" "),a("h4",{attrs:{id:"returns-4"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-4"}},[e._v("#")]),e._v(" Returns")]),e._v(" "),a("p",[e._v("The probabilistic distribution of next action.")]),e._v(" "),a("h3",{attrs:{id:"is-policy-defined-for"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#is-policy-defined-for"}},[e._v("#")]),e._v(" is_policy_defined_for "),a("Badge",{attrs:{text:"Policies",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"is_policy_defined_for",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"bool"}}}),e._v(" "),a("p",[e._v("Check whether the solver's current policy is defined for the given observation.")]),e._v(" "),a("h4",{attrs:{id:"parameters-6"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-6"}},[e._v("#")]),e._v(" Parameters")]),e._v(" "),a("ul",[a("li",[a("strong",[e._v("observation")]),e._v(": The observation to consider.")])]),e._v(" "),a("h4",{attrs:{id:"returns-5"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-5"}},[e._v("#")]),e._v(" Returns")]),e._v(" "),a("p",[e._v("True if the policy is defined for the given observation memory (False otherwise).")]),e._v(" "),a("h3",{attrs:{id:"reset"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#reset"}},[e._v("#")]),e._v(" reset "),a("Badge",{attrs:{text:"Solver",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"reset",sig:{params:[{name:"self"}],return:"None"}}}),e._v(" "),a("p",[e._v("Reset whatever is needed on this solver before running a new episode.")]),e._v(" "),a("p",[e._v("This function does nothing by default but can be overridden if needed (e.g. to reset the hidden state of a LSTM\npolicy network, which carries information about past observations seen in the previous episode).")]),e._v(" "),a("h3",{attrs:{id:"sample-action"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#sample-action"}},[e._v("#")]),e._v(" sample_action "),a("Badge",{attrs:{text:"Policies",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"sample_action",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"D.T_agent[D.T_concurrency[D.T_event]]"}}}),e._v(" "),a("p",[e._v("Sample an action for the given observation (from the solver's current policy).")]),e._v(" "),a("h4",{attrs:{id:"parameters-7"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-7"}},[e._v("#")]),e._v(" Parameters")]),e._v(" "),a("ul",[a("li",[a("strong",[e._v("observation")]),e._v(": The observation for which an action must be sampled.")])]),e._v(" "),a("h4",{attrs:{id:"returns-6"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-6"}},[e._v("#")]),e._v(" Returns")]),e._v(" "),a("p",[e._v("The sampled action.")]),e._v(" "),a("h3",{attrs:{id:"solve"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#solve"}},[e._v("#")]),e._v(" solve "),a("Badge",{attrs:{text:"FromInitialState",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"solve",sig:{params:[{name:"self"}],return:"None"}}}),e._v(" "),a("p",[e._v("Run the solving process.")]),e._v(" "),a("p",[e._v("After solving by calling self._solve(), autocast itself so that rollout methods apply\nto the domain original characteristics.")]),e._v(" "),a("div",{staticClass:"custom-block tip"},[a("p",{staticClass:"custom-block-title"},[e._v("TIP")]),e._v(" "),a("p",[e._v("The nature of the solutions produced here depends on other solver's characteristics like\n"),a("code",[e._v("policy")]),e._v(" and "),a("code",[e._v("assessibility")]),e._v(".")])]),e._v(" "),a("h3",{attrs:{id:"check-domain-additional"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#check-domain-additional"}},[e._v("#")]),e._v(" _check_domain_additional "),a("Badge",{attrs:{text:"Solver",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"_check_domain_additional",sig:{params:[{name:"domain",annotation:"Domain"}],return:"bool"}}}),e._v(" "),a("p",[e._v('Check whether the given domain is compliant with the specific requirements of this solver type (i.e. the\nones in addition to "domain requirements").')]),e._v(" "),a("p",[e._v("This is a helper function called by default from "),a("code",[e._v("Solver.check_domain()")]),e._v(". It focuses on specific checks, as\nopposed to taking also into account the domain requirements for the latter.")]),e._v(" "),a("h4",{attrs:{id:"parameters-8"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-8"}},[e._v("#")]),e._v(" Parameters")]),e._v(" "),a("ul",[a("li",[a("strong",[e._v("domain")]),e._v(": The domain to check.")])]),e._v(" "),a("h4",{attrs:{id:"returns-7"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-7"}},[e._v("#")]),e._v(" Returns")]),e._v(" "),a("p",[e._v("True if the domain is compliant with the specific requirements of this solver type (False otherwise).")]),e._v(" "),a("h3",{attrs:{id:"cleanup"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#cleanup"}},[e._v("#")]),e._v(" _cleanup "),a("Badge",{attrs:{text:"Solver",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"_cleanup",sig:{params:[{name:"self"}]}}}),e._v(" "),a("p",[e._v("Runs cleanup code here, or code to be executed at the exit of a\n'with' context statement.")]),e._v(" "),a("h3",{attrs:{id:"get-best-action"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#get-best-action"}},[e._v("#")]),e._v(" _get_best_action "),a("Badge",{attrs:{text:"POMCP",type:"tip"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"_get_best_action",sig:{params:[{name:"self"},{name:"h_act"},{name:"h_obs"},{name:"w",default:"0"}]}}}),e._v(" "),a("p",[e._v("Retrieve best action at (h_act, h_obs) from stored tree.")]),e._v(" "),a("p",[e._v("If w > 0, best is determined using the UCT formula with weight w;\nelse it's just the action with min expected cost.")]),e._v(" "),a("h3",{attrs:{id:"get-next-action-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#get-next-action-2"}},[e._v("#")]),e._v(" _get_next_action "),a("Badge",{attrs:{text:"DeterministicPolicies",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"_get_next_action",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"D.T_agent[D.T_concurrency[D.T_event]]"}}}),e._v(" "),a("p",[e._v("Get the next deterministic action (from the solver's current policy).")]),e._v(" "),a("h4",{attrs:{id:"parameters-9"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-9"}},[e._v("#")]),e._v(" Parameters")]),e._v(" "),a("ul",[a("li",[a("strong",[e._v("observation")]),e._v(": The observation for which next action is requested.")])]),e._v(" "),a("h4",{attrs:{id:"returns-8"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-8"}},[e._v("#")]),e._v(" Returns")]),e._v(" "),a("p",[e._v("The next deterministic action.")]),e._v(" "),a("h3",{attrs:{id:"get-next-action-distribution-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#get-next-action-distribution-2"}},[e._v("#")]),e._v(" _get_next_action_distribution "),a("Badge",{attrs:{text:"UncertainPolicies",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"_get_next_action_distribution",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"Distribution[D.T_agent[D.T_concurrency[D.T_event]]]"}}}),e._v(" "),a("p",[e._v("Get the probabilistic distribution of next action for the given observation (from the solver's current\npolicy).")]),e._v(" "),a("h4",{attrs:{id:"parameters-10"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-10"}},[e._v("#")]),e._v(" Parameters")]),e._v(" "),a("ul",[a("li",[a("strong",[e._v("observation")]),e._v(": The observation to consider.")])]),e._v(" "),a("h4",{attrs:{id:"returns-9"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-9"}},[e._v("#")]),e._v(" Returns")]),e._v(" "),a("p",[e._v("The probabilistic distribution of next action.")]),e._v(" "),a("h3",{attrs:{id:"initialize"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#initialize"}},[e._v("#")]),e._v(" _initialize "),a("Badge",{attrs:{text:"Solver",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"_initialize",sig:{params:[{name:"self"}]}}}),e._v(" "),a("p",[e._v("Runs long-lasting initialization code here.")]),e._v(" "),a("h3",{attrs:{id:"is-policy-defined-for-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#is-policy-defined-for-2"}},[e._v("#")]),e._v(" _is_policy_defined_for "),a("Badge",{attrs:{text:"Policies",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"_is_policy_defined_for",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"bool"}}}),e._v(" "),a("p",[e._v("Check whether the solver's current policy is defined for the given observation.")]),e._v(" "),a("h4",{attrs:{id:"parameters-11"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-11"}},[e._v("#")]),e._v(" Parameters")]),e._v(" "),a("ul",[a("li",[a("strong",[e._v("observation")]),e._v(": The observation to consider.")])]),e._v(" "),a("h4",{attrs:{id:"returns-10"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-10"}},[e._v("#")]),e._v(" Returns")]),e._v(" "),a("p",[e._v("True if the policy is defined for the given observation memory (False otherwise).")]),e._v(" "),a("h3",{attrs:{id:"reset-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#reset-2"}},[e._v("#")]),e._v(" _reset "),a("Badge",{attrs:{text:"Solver",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"_reset",sig:{params:[{name:"self"}],return:"None"}}}),e._v(" "),a("p",[e._v("Reset whatever is needed on this solver before running a new episode.")]),e._v(" "),a("p",[e._v("This function does nothing by default but can be overridden if needed (e.g. to reset the hidden state of a LSTM\npolicy network, which carries information about past observations seen in the previous episode).")]),e._v(" "),a("h3",{attrs:{id:"sample-action-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#sample-action-2"}},[e._v("#")]),e._v(" _sample_action "),a("Badge",{attrs:{text:"Policies",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"_sample_action",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"D.T_agent[D.T_concurrency[D.T_event]]"}}}),e._v(" "),a("p",[e._v("Sample an action for the given observation (from the solver's current policy).")]),e._v(" "),a("h4",{attrs:{id:"parameters-12"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-12"}},[e._v("#")]),e._v(" Parameters")]),e._v(" "),a("ul",[a("li",[a("strong",[e._v("observation")]),e._v(": The observation for which an action must be sampled.")])]),e._v(" "),a("h4",{attrs:{id:"returns-11"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-11"}},[e._v("#")]),e._v(" Returns")]),e._v(" "),a("p",[e._v("The sampled action.")]),e._v(" "),a("h3",{attrs:{id:"solve-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#solve-2"}},[e._v("#")]),e._v(" _solve "),a("Badge",{attrs:{text:"FromInitialState",type:"warn"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"_solve",sig:{params:[{name:"self"}],return:"None"}}}),e._v(" "),a("p",[e._v("Run the solving process.")]),e._v(" "),a("div",{staticClass:"custom-block tip"},[a("p",{staticClass:"custom-block-title"},[e._v("TIP")]),e._v(" "),a("p",[e._v("The nature of the solutions produced here depends on other solver's characteristics like\n"),a("code",[e._v("policy")]),e._v(" and "),a("code",[e._v("assessibility")]),e._v(".")])]),e._v(" "),a("h3",{attrs:{id:"tree-search"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#tree-search"}},[e._v("#")]),e._v(" _tree_search "),a("Badge",{attrs:{text:"POMCP",type:"tip"}})],1),e._v(" "),a("skdecide-signature",{attrs:{name:"_tree_search",sig:{params:[{name:"self"},{name:"state"},{name:"h_act"},{name:"h_obs"},{name:"depth"}]}}}),e._v(" "),a("p",[e._v("UCT search from a given state with act/obs history.")]),e._v(" "),a("p",[e._v("This corresponds to the Simulate function in the POMCP paper.")]),e._v(" "),a("h2",{attrs:{id:"get-probability"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#get-probability"}},[e._v("#")]),e._v(" get_probability")]),e._v(" "),a("skdecide-signature",{attrs:{name:"get_probability",sig:{params:[{name:"distribution"},{name:"element"},{name:"n",default:"100"}]}}}),e._v(" "),a("p",[e._v("Utility function to get the probability of a specific element from a scikit-decide distribution\n(based on sampling if this distribution is not a DiscreteDistribution).")])],1)}),[],!1,null,null,null);t.default=r.exports}}]);