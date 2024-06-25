(window.webpackJsonp=window.webpackJsonp||[]).push([[129],{639:function(t,e,a){"use strict";a.r(e);var o=a(38),n=Object(o.a)({},(function(){var t=this,e=t.$createElement,a=t._self._c||e;return a("ContentSlotsDistributor",{attrs:{"slot-key":t.$parent.slotKey}},[a("h1",{attrs:{id:"utils"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#utils"}},[t._v("#")]),t._v(" utils")]),t._v(" "),a("p",[t._v("This module contains utility functions.")]),t._v(" "),a("div",{staticClass:"custom-block tip"},[a("p",{staticClass:"custom-block-title"},[t._v("Domain specification")]),t._v(" "),a("skdecide-summary")],1),t._v(" "),a("h2",{attrs:{id:"get-data-home"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#get-data-home"}},[t._v("#")]),t._v(" get_data_home")]),t._v(" "),a("skdecide-signature",{attrs:{name:"get_data_home",sig:{params:[{name:"data_home",default:"None",annotation:"Optional[str]"}],return:"str"}}}),t._v(" "),a("p",[t._v("Return the path of the scikit-decide data directory.")]),t._v(" "),a("p",[t._v("This folder is used by some large dataset loaders to avoid downloading the\ndata several times, as for instance the weather data used by the flight planning domain.\nBy default the data dir is set to a folder named 'skdecide_data' in the\nuser home folder.\nAlternatively, it can be set by the 'SKDECIDE_DATA' environment\nvariable or programmatically by giving an explicit folder path. The '~'\nsymbol is expanded to the user home folder.\nIf the folder does not already exist, it is automatically created.")]),t._v(" "),a("p",[t._v("Params:\ndata_home : The path to scikit-decide data directory. If "),a("code",[t._v("None")]),t._v(", the default path\nis "),a("code",[t._v("~/skdecide_data")]),t._v(".")]),t._v(" "),a("h2",{attrs:{id:"rollout"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#rollout"}},[t._v("#")]),t._v(" rollout")]),t._v(" "),a("skdecide-signature",{attrs:{name:"rollout",sig:{params:[{name:"domain",annotation:"Domain"},{name:"solver",default:"None",annotation:"Optional[Union[Solver, Policies]]"},{name:"from_memory",default:"None",annotation:"Optional[D.T_memory[D.T_state]]"},{name:"from_action",default:"None",annotation:"Optional[D.T_agent[D.T_concurrency[D.T_event]]]"},{name:"num_episodes",default:"1",annotation:"int"},{name:"max_steps",default:"None",annotation:"Optional[int]"},{name:"render",default:"True",annotation:"bool"},{name:"max_framerate",default:"None",annotation:"Optional[float]"},{name:"verbose",default:"True",annotation:"bool"},{name:"action_formatter",default:"<lambda function>",annotation:"Optional[Callable[[D.T_event], str]]"},{name:"outcome_formatter",default:"<lambda function>",annotation:"Optional[Callable[[EnvironmentOutcome], str]]"},{name:"return_episodes",default:"False",annotation:"bool"},{name:"goal_logging_level",default:"20",annotation:"int"},{name:"rollout_callback",default:"None",annotation:"Optional[RolloutCallback]"}],return:"Optional[List[Tuple[List[D.T_observation], List[D.T_event], List[D.T_value]]]]"}}}),t._v(" "),a("p",[t._v("This method will run one or more episodes in a domain according to the policy of a solver.")]),t._v(" "),a("h4",{attrs:{id:"parameters"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters"}},[t._v("#")]),t._v(" Parameters")]),t._v(" "),a("ul",[a("li",[a("strong",[t._v("domain")]),t._v(": The domain in which the episode(s) will be run.")]),t._v(" "),a("li",[a("strong",[t._v("solver")]),t._v(": The solver whose policy will select actions to take (if None, a random policy is used).")]),t._v(" "),a("li",[a("strong",[t._v("from_memory")]),t._v(": The memory or state to consider as rollout starting point (if None, the domain is reset first).")]),t._v(" "),a("li",[a("strong",[t._v("from_action")]),t._v(": The last applied action when from_memory is used (if necessary for initial observation computation).")]),t._v(" "),a("li",[a("strong",[t._v("num_episodes")]),t._v(": The number of episodes to run.")]),t._v(" "),a("li",[a("strong",[t._v("max_steps")]),t._v(": The maximum number of steps for each episode (if None, no limit is set).")]),t._v(" "),a("li",[a("strong",[t._v("render")]),t._v(": Whether to render the episode(s) during rollout if the domain is renderable.")]),t._v(" "),a("li",[a("strong",[t._v("max_framerate")]),t._v(": The maximum number of steps/renders per second (if None, steps/renders are never slowed down).")]),t._v(" "),a("li",[a("strong",[t._v("verbose")]),t._v(": Whether to print information to the console during rollout.")]),t._v(" "),a("li",[a("strong",[t._v("action_formatter")]),t._v(": The function transforming actions in the string to print (if None, no print).")]),t._v(" "),a("li",[a("strong",[t._v("outcome_formatter")]),t._v(": The function transforming EnvironmentOutcome objects in the string to print (if None, no print).")]),t._v(" "),a("li",[a("strong",[t._v("return_episodes")]),t._v(": if True, return the list of episodes, each episode as a tuple of observations, actions, and values.\nelse return nothing.")]),t._v(" "),a("li",[a("strong",[t._v("goal_logging_level")]),t._v(": logging level at which we want to display if goal has been reached or not")])]),t._v(" "),a("h2",{attrs:{id:"rolloutcallback"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#rolloutcallback"}},[t._v("#")]),t._v(" RolloutCallback")]),t._v(" "),a("p",[t._v("Callback used during rollout to add custom behaviour.")]),t._v(" "),a("p",[t._v("One should derives from this one in order to hook in different stages of the rollout.")]),t._v(" "),a("h3",{attrs:{id:"at-episode-end"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#at-episode-end"}},[t._v("#")]),t._v(" at_episode_end "),a("Badge",{attrs:{text:"RolloutCallback",type:"tip"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"at_episode_end",sig:{params:[{name:"self"}]}}}),t._v(" "),a("p",[t._v("Called after each episode.")]),t._v(" "),a("h3",{attrs:{id:"at-episode-start"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#at-episode-start"}},[t._v("#")]),t._v(" at_episode_start "),a("Badge",{attrs:{text:"RolloutCallback",type:"tip"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"at_episode_start",sig:{params:[{name:"self"}]}}}),t._v(" "),a("p",[t._v("Called before each episode.")]),t._v(" "),a("h3",{attrs:{id:"at-episode-step"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#at-episode-step"}},[t._v("#")]),t._v(" at_episode_step "),a("Badge",{attrs:{text:"RolloutCallback",type:"tip"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"at_episode_step",sig:{params:[{name:"self"},{name:"i_episode",annotation:"int"},{name:"step",annotation:"int"},{name:"domain",annotation:"Domain"},{name:"solver",annotation:"Union[Solver, Policies]"},{name:"action",annotation:"D.T_agent[D.T_concurrency[D.T_event]]"},{name:"outcome",annotation:"EnvironmentOutcome[D.T_agent[D.T_observation], D.T_agent[Value[D.T_value]], D.T_agent[D.T_predicate], D.T_agent[D.T_info]]"}],return:"bool"}}}),t._v(" "),a("h4",{attrs:{id:"parameters-2"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#parameters-2"}},[t._v("#")]),t._v(" Parameters")]),t._v(" "),a("ul",[a("li",[a("strong",[t._v("i_episode")]),t._v(": current episode number")]),t._v(" "),a("li",[a("strong",[t._v("step")]),t._v(": current step number within the episode")]),t._v(" "),a("li",[a("strong",[t._v("domain")]),t._v(": domain considered")]),t._v(" "),a("li",[a("strong",[t._v("solver")]),t._v(": solver considered (or randomwalk policy if solver was None in rollout)")]),t._v(" "),a("li",[a("strong",[t._v("action")]),t._v(": last action sampled")]),t._v(" "),a("li",[a("strong",[t._v("outcome")]),t._v(": outcome of the last action applied to the domain")])]),t._v(" "),a("h4",{attrs:{id:"returns"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#returns"}},[t._v("#")]),t._v(" Returns")]),t._v(" "),a("p",[t._v("stopping: if True, the rollout for the current episode stops and the next episode starts.")]),t._v(" "),a("h3",{attrs:{id:"at-rollout-end"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#at-rollout-end"}},[t._v("#")]),t._v(" at_rollout_end "),a("Badge",{attrs:{text:"RolloutCallback",type:"tip"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"at_rollout_end",sig:{params:[{name:"self"}]}}}),t._v(" "),a("p",[t._v("Called at rollout end.")]),t._v(" "),a("h3",{attrs:{id:"at-rollout-start"}},[a("a",{staticClass:"header-anchor",attrs:{href:"#at-rollout-start"}},[t._v("#")]),t._v(" at_rollout_start "),a("Badge",{attrs:{text:"RolloutCallback",type:"tip"}})],1),t._v(" "),a("skdecide-signature",{attrs:{name:"at_rollout_start",sig:{params:[{name:"self"}]}}}),t._v(" "),a("p",[t._v("Called at rollout start.")])],1)}),[],!1,null,null,null);e.default=n.exports}}]);