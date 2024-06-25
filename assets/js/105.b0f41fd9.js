(window.webpackJsonp=window.webpackJsonp||[]).push([[105],{616:function(t,e,a){"use strict";a.r(e);var s=a(38),r=Object(s.a)({},(function(){var t=this,e=t.$createElement,a=t._self._c||e;return a("ContentSlotsDistributor",{attrs:{"slot-key":t.$parent.slotKey}},[a("h1",{attrs:{id:"hub-solver-do-solver-sgs-policies"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#hub-solver-do-solver-sgs-policies"}},[t._v("#")]),t._v(" hub.solver.do_solver.sgs_policies")]),t._v(" "),a("div",{staticClass:"custom-block tip"},[a("p",{staticClass:"custom-block-title"},[t._v("Domain specification")]),t._v(" "),a("skdecide-summary")],1),t._v(" "),a("h2",{attrs:{id:"basepolicymethod"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#basepolicymethod"}},[t._v("#")]),t._v(" BasePolicyMethod")]),t._v(" "),a("p",[t._v("Base options to define Scheduling policies")]),t._v(" "),a("h3",{attrs:{id:"follow-gantt"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#follow-gantt"}},[t._v("#")]),t._v(" FOLLOW_GANTT "),a("Badge",{attrs:{text:"BasePolicyMethod",type:"tip"}})],1),t._v(" "),a("p",[t._v("Strictly return scheduling policy based on the gantt chart.\nBased on the time stored in the state, task are started at the right time.")]),t._v(" "),a("h3",{attrs:{id:"sgs-index-freedom"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#sgs-index-freedom"}},[t._v("#")]),t._v(" SGS_INDEX_FREEDOM "),a("Badge",{attrs:{text:"BasePolicyMethod",type:"tip"}})],1),t._v(" "),a("p",[t._v('At a given state, look for the first task "TASK" in the permutation that is not started or scheduled yet,\nIf it\'s not available to start yet, some other task are considered candidates based their "ordering"\ncloseness to the starting time of "TASK", the policy will consider starting task that are close to\nthe one that was first expected. '),a("code",[t._v("delta_index_freedom")]),t._v(" is the parameter that impacts this setting.")]),t._v(" "),a("h3",{attrs:{id:"sgs-precedence"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#sgs-precedence"}},[t._v("#")]),t._v(" SGS_PRECEDENCE "),a("Badge",{attrs:{text:"BasePolicyMethod",type:"tip"}})],1),t._v(" "),a("p",[t._v("At a given state, look for the first available task\nin an ordered permutation that is start-able and do it.\nIf no activity is launchable, just advance in time.")]),t._v(" "),a("h3",{attrs:{id:"sgs-ready"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#sgs-ready"}},[t._v("#")]),t._v(" SGS_READY "),a("Badge",{attrs:{text:"BasePolicyMethod",type:"tip"}})],1),t._v(" "),a("p",[t._v("Same as SGS_PRECEDENCE, one of those 2 will be in deprecation soon.")]),t._v(" "),a("h3",{attrs:{id:"sgs-strict"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#sgs-strict"}},[t._v("#")]),t._v(" SGS_STRICT "),a("Badge",{attrs:{text:"BasePolicyMethod",type:"tip"}})],1),t._v(" "),a("p",[t._v("At a given state, look for the first task in the permutation that is not started or scheduled yet.\nIf it's not available to start yet, we advance in time until it is.")]),t._v(" "),a("div",{staticClass:"custom-block warning"},[a("p",{staticClass:"custom-block-title"},[t._v("WARNING")]),t._v(" "),a("p",[t._v("This will only work when the permutation of tasks fulfills the precedence constraints.")])]),t._v(" "),a("h3",{attrs:{id:"sgs-time-freedom"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#sgs-time-freedom"}},[t._v("#")]),t._v(" SGS_TIME_FREEDOM "),a("Badge",{attrs:{text:"BasePolicyMethod",type:"tip"}})],1),t._v(" "),a("p",[t._v('At a given state, look for the first task "TASK" in the permutation that is not started or scheduled yet,\nIf it\'s not available to start yet, some other task are considered candidates based their time\ncloseness to the starting time of "TASK", the policy will consider starting task that are close to\nthe one that was first expected. '),a("code",[t._v("delta_time_freedom")]),t._v(" is the parameter that impacts this setting.")]),t._v(" "),a("h2",{attrs:{id:"policymethodparams"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#policymethodparams"}},[t._v("#")]),t._v(" PolicyMethodParams")]),t._v(" "),a("p",[t._v("Wrapped params for scheduling policy parameters, see BasePolicyMethod for more details")]),t._v(" "),a("h3",{attrs:{id:"constructor"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#constructor"}},[t._v("#")]),t._v(" Constructor "),a("Badge",{attrs:{text:"PolicyMethodParams",type:"tip"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"PolicyMethodParams",sig:{params:[{name:"base_policy_method",annotation:"BasePolicyMethod"},{name:"delta_time_freedom",default:"10"},{name:"delta_index_freedom",default:"10"}]}}}),t._v(" "),a("p",[t._v("Initialize self.  See help(type(self)) for accurate signature.")]),t._v(" "),a("h2",{attrs:{id:"policyrcpsp"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#policyrcpsp"}},[t._v("#")]),t._v(" PolicyRCPSP")]),t._v(" "),a("p",[t._v("Policy object containing results of scheduling solver policy.")]),t._v(" "),a("h4",{attrs:{id:"attributes"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#attributes"}},[t._v("#")]),t._v(" Attributes")]),t._v(" "),a("ul",[a("li",[a("strong",[t._v("domain")]),t._v(": scheduling domain where the policy will be applied")]),t._v(" "),a("li",[a("strong",[t._v("policy_method_params")]),t._v(": params of the policy")]),t._v(" "),a("li",[a("strong",[t._v("permutation_task")]),t._v(": list of tasks ids, representing a priority list for scheduling")]),t._v(" "),a("li",[a("strong",[t._v("modes_dictionnary")]),t._v(": when relevant (multimode rcpsp for e.g) specifies in which mode a task is executed")]),t._v(" "),a("li",[a("strong",[t._v("schedule")]),t._v(": when given, details the schedule to follow : this will be relevant for deterministic scheduling problems")]),t._v(" "),a("li",[a("strong",[t._v("resource_allocation")]),t._v(": when relevant (multiskill problems for e.g), list the allocated (unitary) resources to the tasks")]),t._v(" "),a("li",[a("strong",[t._v("resource_allocation_priority")]),t._v(": for each task, store a preference order for resources to be allocated to the task.\nResource will be greedily allocated based on this priority")])]),t._v(" "),a("h3",{attrs:{id:"constructor-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#constructor-2"}},[t._v("#")]),t._v(" Constructor "),a("Badge",{attrs:{text:"PolicyRCPSP",type:"tip"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"PolicyRCPSP",sig:{params:[{name:"domain",annotation:"SchedulingDomain"},{name:"policy_method_params",annotation:"PolicyMethodParams"},{name:"permutation_task",annotation:"List[int]"},{name:"modes_dictionnary",annotation:"Dict[int, int]"},{name:"schedule",default:"None",annotation:"Optional[Dict[int, Dict[str, int]]]"},{name:"resource_allocation",default:"None",annotation:"Optional[Dict[int, List[str]]]"},{name:"resource_allocation_priority",default:"None",annotation:"Optional[Dict[int, List[str]]]"}]}}}),t._v(" "),a("p",[t._v("Initialize self.  See help(type(self)) for accurate signature.")]),t._v(" "),a("h3",{attrs:{id:"get-next-action"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#get-next-action"}},[t._v("#")]),t._v(" get_next_action "),a("Badge",{attrs:{text:"DeterministicPolicies",type:"warn"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"get_next_action",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"D.T_agent[D.T_concurrency[D.T_event]]"}}}),t._v(" "),a("p",[t._v("Get the next deterministic action (from the solver's current policy).")]),t._v(" "),a("h4",{attrs:{id:"parameters"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters"}},[t._v("#")]),t._v(" Parameters")]),t._v(" "),a("ul",[a("li",[a("strong",[t._v("observation")]),t._v(": The observation for which next action is requested.")])]),t._v(" "),a("h4",{attrs:{id:"returns"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns"}},[t._v("#")]),t._v(" Returns")]),t._v(" "),a("p",[t._v("The next deterministic action.")]),t._v(" "),a("h3",{attrs:{id:"get-next-action-distribution"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#get-next-action-distribution"}},[t._v("#")]),t._v(" get_next_action_distribution "),a("Badge",{attrs:{text:"UncertainPolicies",type:"warn"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"get_next_action_distribution",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"Distribution[D.T_agent[D.T_concurrency[D.T_event]]]"}}}),t._v(" "),a("p",[t._v("Get the probabilistic distribution of next action for the given observation (from the solver's current\npolicy).")]),t._v(" "),a("h4",{attrs:{id:"parameters-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-2"}},[t._v("#")]),t._v(" Parameters")]),t._v(" "),a("ul",[a("li",[a("strong",[t._v("observation")]),t._v(": The observation to consider.")])]),t._v(" "),a("h4",{attrs:{id:"returns-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-2"}},[t._v("#")]),t._v(" Returns")]),t._v(" "),a("p",[t._v("The probabilistic distribution of next action.")]),t._v(" "),a("h3",{attrs:{id:"is-policy-defined-for"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#is-policy-defined-for"}},[t._v("#")]),t._v(" is_policy_defined_for "),a("Badge",{attrs:{text:"Policies",type:"warn"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"is_policy_defined_for",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"bool"}}}),t._v(" "),a("p",[t._v("Check whether the solver's current policy is defined for the given observation.")]),t._v(" "),a("h4",{attrs:{id:"parameters-3"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-3"}},[t._v("#")]),t._v(" Parameters")]),t._v(" "),a("ul",[a("li",[a("strong",[t._v("observation")]),t._v(": The observation to consider.")])]),t._v(" "),a("h4",{attrs:{id:"returns-3"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-3"}},[t._v("#")]),t._v(" Returns")]),t._v(" "),a("p",[t._v("True if the policy is defined for the given observation memory (False otherwise).")]),t._v(" "),a("h3",{attrs:{id:"sample-action"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#sample-action"}},[t._v("#")]),t._v(" sample_action "),a("Badge",{attrs:{text:"Policies",type:"warn"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"sample_action",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"D.T_agent[D.T_concurrency[D.T_event]]"}}}),t._v(" "),a("p",[t._v("Sample an action for the given observation (from the solver's current policy).")]),t._v(" "),a("h4",{attrs:{id:"parameters-4"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-4"}},[t._v("#")]),t._v(" Parameters")]),t._v(" "),a("ul",[a("li",[a("strong",[t._v("observation")]),t._v(": The observation for which an action must be sampled.")])]),t._v(" "),a("h4",{attrs:{id:"returns-4"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-4"}},[t._v("#")]),t._v(" Returns")]),t._v(" "),a("p",[t._v("The sampled action.")]),t._v(" "),a("h3",{attrs:{id:"get-next-action-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#get-next-action-2"}},[t._v("#")]),t._v(" _get_next_action "),a("Badge",{attrs:{text:"DeterministicPolicies",type:"warn"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"_get_next_action",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"D.T_agent[D.T_concurrency[D.T_event]]"}}}),t._v(" "),a("p",[t._v("Get the next deterministic action (from the solver's current policy).")]),t._v(" "),a("h4",{attrs:{id:"parameters-5"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-5"}},[t._v("#")]),t._v(" Parameters")]),t._v(" "),a("ul",[a("li",[a("strong",[t._v("observation")]),t._v(": The observation for which next action is requested.")])]),t._v(" "),a("h4",{attrs:{id:"returns-5"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-5"}},[t._v("#")]),t._v(" Returns")]),t._v(" "),a("p",[t._v("The next deterministic action.")]),t._v(" "),a("h3",{attrs:{id:"get-next-action-distribution-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#get-next-action-distribution-2"}},[t._v("#")]),t._v(" _get_next_action_distribution "),a("Badge",{attrs:{text:"UncertainPolicies",type:"warn"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"_get_next_action_distribution",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"Distribution[D.T_agent[D.T_concurrency[D.T_event]]]"}}}),t._v(" "),a("p",[t._v("Get the probabilistic distribution of next action for the given observation (from the solver's current\npolicy).")]),t._v(" "),a("h4",{attrs:{id:"parameters-6"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-6"}},[t._v("#")]),t._v(" Parameters")]),t._v(" "),a("ul",[a("li",[a("strong",[t._v("observation")]),t._v(": The observation to consider.")])]),t._v(" "),a("h4",{attrs:{id:"returns-6"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-6"}},[t._v("#")]),t._v(" Returns")]),t._v(" "),a("p",[t._v("The probabilistic distribution of next action.")]),t._v(" "),a("h3",{attrs:{id:"is-policy-defined-for-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#is-policy-defined-for-2"}},[t._v("#")]),t._v(" _is_policy_defined_for "),a("Badge",{attrs:{text:"Policies",type:"warn"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"_is_policy_defined_for",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"bool"}}}),t._v(" "),a("p",[t._v("Check whether the solver's current policy is defined for the given observation.")]),t._v(" "),a("h4",{attrs:{id:"parameters-7"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-7"}},[t._v("#")]),t._v(" Parameters")]),t._v(" "),a("ul",[a("li",[a("strong",[t._v("observation")]),t._v(": The observation to consider.")])]),t._v(" "),a("h4",{attrs:{id:"returns-7"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-7"}},[t._v("#")]),t._v(" Returns")]),t._v(" "),a("p",[t._v("True if the policy is defined for the given observation memory (False otherwise).")]),t._v(" "),a("h3",{attrs:{id:"sample-action-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#sample-action-2"}},[t._v("#")]),t._v(" _sample_action "),a("Badge",{attrs:{text:"Policies",type:"warn"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"_sample_action",sig:{params:[{name:"self"},{name:"observation",annotation:"D.T_agent[D.T_observation]"}],return:"D.T_agent[D.T_concurrency[D.T_event]]"}}}),t._v(" "),a("p",[t._v("Sample an action for the given observation (from the solver's current policy).")]),t._v(" "),a("h4",{attrs:{id:"parameters-8"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-8"}},[t._v("#")]),t._v(" Parameters")]),t._v(" "),a("ul",[a("li",[a("strong",[t._v("observation")]),t._v(": The observation for which an action must be sampled.")])]),t._v(" "),a("h4",{attrs:{id:"returns-8"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns-8"}},[t._v("#")]),t._v(" Returns")]),t._v(" "),a("p",[t._v("The sampled action.")]),t._v(" "),a("h2",{attrs:{id:"next-action-follow-static-gantt"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#next-action-follow-static-gantt"}},[t._v("#")]),t._v(" next_action_follow_static_gantt")]),t._v(" "),a("skdecide-signature",{attrs:{name:"next_action_follow_static_gantt",sig:{params:[{name:"policy_rcpsp",annotation:"PolicyRCPSP"},{name:"state",annotation:"State"},{name:"check_if_applicable",default:"False",annotation:"bool"},{name:"**kwargs"}]}}}),t._v(" "),a("p",[t._v("Implements the policy with the parameters FOLLOW_GANTT (see its doc)")]),t._v(" "),a("h2",{attrs:{id:"next-action-sgs-first-task-precedence-ready"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#next-action-sgs-first-task-precedence-ready"}},[t._v("#")]),t._v(" next_action_sgs_first_task_precedence_ready")]),t._v(" "),a("skdecide-signature",{attrs:{name:"next_action_sgs_first_task_precedence_ready",sig:{params:[{name:"policy_rcpsp",annotation:"PolicyRCPSP"},{name:"state",annotation:"State"},{name:"check_if_applicable",default:"False",annotation:"bool"},{name:"**kwargs"}]}}}),t._v(" "),a("p",[t._v("Implements the policy with the parameters SGS_PRECEDENCE (see its doc)")]),t._v(" "),a("h2",{attrs:{id:"next-action-sgs-first-task-ready"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#next-action-sgs-first-task-ready"}},[t._v("#")]),t._v(" next_action_sgs_first_task_ready")]),t._v(" "),a("skdecide-signature",{attrs:{name:"next_action_sgs_first_task_ready",sig:{params:[{name:"policy_rcpsp",annotation:"PolicyRCPSP"},{name:"state",annotation:"State"},{name:"check_if_applicable",default:"False",annotation:"bool"},{name:"domain_sk_decide",default:"None",annotation:"Union[MultiModeRCPSP, SingleModeRCPSP]"},{name:"**kwargs"}]}}}),t._v(" "),a("p",[t._v("Implements the policy with the parameters SGS_READY (see its doc)")]),t._v(" "),a("h2",{attrs:{id:"next-action-sgs-strict"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#next-action-sgs-strict"}},[t._v("#")]),t._v(" next_action_sgs_strict")]),t._v(" "),a("skdecide-signature",{attrs:{name:"next_action_sgs_strict",sig:{params:[{name:"policy_rcpsp",annotation:"PolicyRCPSP"},{name:"state",annotation:"State"},{name:"check_if_applicable",default:"False",annotation:"bool"},{name:"domain_sk_decide",default:"None",annotation:"Union[MultiModeRCPSP, SingleModeRCPSP]"},{name:"**kwargs"}]}}}),t._v(" "),a("p",[t._v("Implements the policy with the parameters SGS_STRICT (see its doc)")]),t._v(" "),a("h2",{attrs:{id:"next-action-sgs-time-freedom"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#next-action-sgs-time-freedom"}},[t._v("#")]),t._v(" next_action_sgs_time_freedom")]),t._v(" "),a("skdecide-signature",{attrs:{name:"next_action_sgs_time_freedom",sig:{params:[{name:"policy_rcpsp",annotation:"PolicyRCPSP"},{name:"state",annotation:"State"},{name:"check_if_applicable",default:"False",annotation:"bool"},{name:"domain_sk_decide",default:"None",annotation:"Union[MultiModeRCPSP, SingleModeRCPSP]"},{name:"delta_time_freedom",default:"10",annotation:"int"},{name:"**kwargs"}]}}}),t._v(" "),a("p",[t._v("Implements the policy with the parameters SGS_TIME_FREEDOM (see its doc)")]),t._v(" "),a("h2",{attrs:{id:"next-action-sgs-index-freedom"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#next-action-sgs-index-freedom"}},[t._v("#")]),t._v(" next_action_sgs_index_freedom")]),t._v(" "),a("skdecide-signature",{attrs:{name:"next_action_sgs_index_freedom",sig:{params:[{name:"policy_rcpsp",annotation:"PolicyRCPSP"},{name:"state",annotation:"State"},{name:"check_if_applicable",default:"False",annotation:"bool"},{name:"domain_sk_decide",default:"None",annotation:"Union[MultiModeRCPSP, SingleModeRCPSP]"},{name:"delta_index_freedom",default:"10",annotation:"int"},{name:"**kwargs"}]}}}),t._v(" "),a("p",[t._v("Implements the policy with the parameters SGS_INDEX_FREEDOM (see its doc)")])],1)}),[],!1,null,null,null);e.default=r.exports}}]);