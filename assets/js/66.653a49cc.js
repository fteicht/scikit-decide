(window.webpackJsonp=window.webpackJsonp||[]).push([[66],{578:function(a,t,e){"use strict";e.r(t);var r=e(38),s=Object(r.a)({},(function(){var a=this,t=a.$createElement,e=a._self._c||t;return e("ContentSlotsDistributor",{attrs:{"slot-key":a.$parent.slotKey}},[e("h1",{attrs:{id:"hub-domain-flight-planning-aircraft-performance-poll-schumann-utils-parameters-jet"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#hub-domain-flight-planning-aircraft-performance-poll-schumann-utils-parameters-jet"}},[a._v("#")]),a._v(" hub.domain.flight_planning.aircraft_performance.poll_schumann_utils.parameters.jet")]),a._v(" "),e("div",{staticClass:"custom-block tip"},[e("p",{staticClass:"custom-block-title"},[a._v("Domain specification")]),a._v(" "),e("skdecide-summary")],1),a._v(" "),e("h2",{attrs:{id:"equivalent-fuel-flow-rate-at-cruise"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#equivalent-fuel-flow-rate-at-cruise"}},[a._v("#")]),a._v(" equivalent_fuel_flow_rate_at_cruise")]),a._v(" "),e("skdecide-signature",{attrs:{name:"equivalent_fuel_flow_rate_at_cruise",sig:{params:[{name:"fuel_flow_sls",annotation:"<class 'float'>"},{name:"theta_amb",annotation:"<class 'float'>"},{name:"delta_amb",annotation:"<class 'float'>"},{name:"mach_num",annotation:"<class 'float'>"}],return:"<class 'float'>"}}}),a._v(" "),e("p",[a._v("Convert fuel mass flow rate at sea level to equivalent fuel flow rate at cruise conditions.")]),a._v(" "),e("h4",{attrs:{id:"parameters"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#parameters"}},[a._v("#")]),a._v(" Parameters")]),a._v(" "),e("p",[a._v("fuel_flow_sls (float):\nFuel mass flow rate at sea level, [:math:"),e("code",[a._v("kg/s")]),a._v("].\ntheta_amb (float):\nAmbient temperature ratio, [:math:"),e("code",[a._v("K")]),a._v("].\ndelta_amb (float):\nAmbient pressure ratio, [:math:"),e("code",[a._v("Pa")]),a._v("].\nmach_num (float):\nMach number, [:math:"),e("code",[a._v("-")]),a._v("].")]),a._v(" "),e("h4",{attrs:{id:"returns"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#returns"}},[a._v("#")]),a._v(" Returns")]),a._v(" "),e("div",{staticClass:"language- extra-class"},[e("pre",[e("code",[a._v("float: Equivalent fuel mass flow rate at cruise conditions, [:math:`kg/s`].\n")])])]),e("h2",{attrs:{id:"clip-mach-number"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#clip-mach-number"}},[a._v("#")]),a._v(" clip_mach_number")]),a._v(" "),e("skdecide-signature",{attrs:{name:"clip_mach_number",sig:{params:[{name:"true_airspeed",annotation:"<class 'float'>"},{name:"air_temperature",annotation:"<class 'float'>"},{name:"max_mach_number",annotation:"<class 'float'>"}]}}}),a._v(" "),e("p",[a._v("Compute the Mach number from the true airspeed and ambient temperature.")]),a._v(" "),e("h4",{attrs:{id:"parameters-2"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#parameters-2"}},[a._v("#")]),a._v(" Parameters")]),a._v(" "),e("p",[a._v("true_airspeed (float):\nTrue airspeed, [:math:"),e("code",[a._v("m/s")]),a._v("].\nair_temperature (float):\nAmbient temperature, [:math:"),e("code",[a._v("K")]),a._v("].\nmax_mach_number (float):\nMaximum permitted operational Mach number for aircraft type.")]),a._v(" "),e("h4",{attrs:{id:"returns-2"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#returns-2"}},[a._v("#")]),a._v(" Returns")]),a._v(" "),e("div",{staticClass:"language- extra-class"},[e("pre",[e("code",[a._v("Tuple[np.ndarray, np.ndarray]: Adjusted true airspeed and Mach number.\n")])])]),e("h2",{attrs:{id:"rate-of-climb-descent"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#rate-of-climb-descent"}},[a._v("#")]),a._v(" rate_of_climb_descent")]),a._v(" "),e("skdecide-signature",{attrs:{name:"rate_of_climb_descent",sig:{params:[{name:"altitude_current",annotation:"<class 'float'>"},{name:"altitude_next",annotation:"<class 'float'>"},{name:"delta_time",annotation:"<class 'float'>"}],return:"<class 'float'>"}}}),a._v(" "),e("p",[a._v("Compute the rate of climb or descent (ft/min) from the path angle and speed.")]),a._v(" "),e("h4",{attrs:{id:"parameters-3"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#parameters-3"}},[a._v("#")]),a._v(" Parameters")]),a._v(" "),e("p",[a._v("altitude_current (float):\nCurrent altitude, [:math:"),e("code",[a._v("ft")]),a._v("].\naltitude_next (float):\nNext altitude, [:math:"),e("code",[a._v("ft")]),a._v("].\ndelta_time (float):\nTime step, [:math:"),e("code",[a._v("s")]),a._v("].")]),a._v(" "),e("h4",{attrs:{id:"returns-3"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#returns-3"}},[a._v("#")]),a._v(" Returns")]),a._v(" "),e("div",{staticClass:"language- extra-class"},[e("pre",[e("code",[a._v("float: Rate of climb or descent, [:math:`ft/min`].\n")])])]),e("h2",{attrs:{id:"acceleration"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#acceleration"}},[a._v("#")]),a._v(" acceleration")]),a._v(" "),e("skdecide-signature",{attrs:{name:"acceleration",sig:{params:[{name:"speed_current"},{name:"delta_time"}],return:"<class 'float'>"}}}),a._v(" "),e("p",[a._v("Calculate the acceleration/deceleration at each waypoint.")]),a._v(" "),e("h4",{attrs:{id:"parameters-4"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#parameters-4"}},[a._v("#")]),a._v(" Parameters")]),a._v(" "),e("p",[a._v("speed_current (float):\nCurrent speed, [:math:"),e("code",[a._v("kts")]),a._v("].\ndelta_time (float):\nTime step, [:math:"),e("code",[a._v("s")]),a._v("].")]),a._v(" "),e("h4",{attrs:{id:"returns-4"}},[e("a",{staticClass:"header-anchor",attrs:{href:"#returns-4"}},[a._v("#")]),a._v(" Returns")]),a._v(" "),e("div",{staticClass:"language- extra-class"},[e("pre",[e("code",[a._v("float: Acceleration, [:math:`kts/s`].\n")])])])],1)}),[],!1,null,null,null);t.default=s.exports}}]);