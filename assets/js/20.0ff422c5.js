(window.webpackJsonp=window.webpackJsonp||[]).push([[20],{632:function(n,e,t){"use strict";t.r(e);var i=t(38),a=Object(i.a)({},(function(){var n=this,e=n.$createElement,t=n._self._c||e;return t("ContentSlotsDistributor",{attrs:{"slot-key":n.$parent.slotKey}},[t("h1",{attrs:{id:"installation"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#installation"}},[n._v("#")]),n._v(" Installation")]),n._v(" "),t("h2",{attrs:{id:"prerequisites"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#prerequisites"}},[n._v("#")]),n._v(" Prerequisites")]),n._v(" "),t("h3",{attrs:{id:"minizinc-2-6"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#minizinc-2-6"}},[n._v("#")]),n._v(" Minizinc 2.6+")]),n._v(" "),t("p",[n._v("You need to install "),t("a",{attrs:{href:"https://www.minizinc.org/",target:"_blank",rel:"noopener noreferrer"}},[n._v("minizinc"),t("OutboundLink")],1),n._v(" (version greater than 2.6) and update the "),t("code",[n._v("PATH")]),n._v(" environment variable\nso that it can be found by Python. See "),t("a",{attrs:{href:"https://www.minizinc.org/doc-latest/en/installation.html",target:"_blank",rel:"noopener noreferrer"}},[n._v("minizinc documentation"),t("OutboundLink")],1),n._v(" for more details.")]),n._v(" "),t("div",{staticClass:"custom-block tip"},[t("p",{staticClass:"custom-block-title"},[n._v("TIP")]),n._v(" "),t("p",[n._v("You can easily install minizinc from the command line, which can be useful when on cloud.\nIn order to make life easier to cloud users, we reproduce below the necessary lines. Please be careful that this\nis not an official documentation for minizinc and that the following lines can stop working without notice\nas we do not test them automatically.")])]),n._v(" "),t("h4",{attrs:{id:"linux-command-line"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#linux-command-line"}},[n._v("#")]),n._v(" Linux command line")]),n._v(" "),t("p",[n._v("On a Linux distribution, you can use the bundled "),t("a",{attrs:{href:"https://www.minizinc.org/doc-latest/en/installation.html#appimage",target:"_blank",rel:"noopener noreferrer"}},[n._v("minizinc AppImage"),t("OutboundLink")],1),n._v(".")]),n._v(" "),t("p",[n._v("If "),t("a",{attrs:{href:"https://en.wikipedia.org/wiki/Filesystem_in_Userspace",target:"_blank",rel:"noopener noreferrer"}},[n._v("FUSE"),t("OutboundLink")],1),n._v(" is available:")]),n._v(" "),t("div",{staticClass:"language- extra-class"},[t("pre",{pre:!0,attrs:{class:"language-text"}},[t("code",[n._v('mkdir minizinc_install\ncurl -o minizinc_install/minizinc -L https://github.com/MiniZinc/MiniZincIDE/releases/download/2.6.3/MiniZincIDE-2.6.3-x86_64.AppImage\nchmod +x minizinc_install/minizinc\nexport PATH="$(pwd)/minizinc_install/":$PATH\nminizinc --version\n')])])]),t("p",[n._v("Else, this is still possible by extracting the files:")]),n._v(" "),t("div",{staticClass:"language- extra-class"},[t("pre",{pre:!0,attrs:{class:"language-text"}},[t("code",[n._v('mkdir minizinc_install\ncd minizinc_install\ncurl -o minizinc.AppImage -L https://github.com/MiniZinc/MiniZincIDE/releases/download/2.6.3/MiniZincIDE-2.6.3-x86_64.AppImage\nchmod +x minizinc.AppImage\n./minizinc.AppImage --appimage-extract\ncd ..\nexport LD_LIBRARY_PATH="$(pwd)/minizinc_install/squashfs-root/usr/lib/":$LD_LIBRARY_PATH\nexport PATH="$(pwd)/minizinc_install/squashfs-root/usr/bin/":$PATH\nminizinc --version\n')])])]),t("h4",{attrs:{id:"macos-command-line"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#macos-command-line"}},[n._v("#")]),n._v(" MacOs command line")]),n._v(" "),t("div",{staticClass:"language- extra-class"},[t("pre",{pre:!0,attrs:{class:"language-text"}},[t("code",[n._v('mkdir minizinc_install\ncurl -o minizinc.dmg -L https://github.com/MiniZinc/MiniZincIDE/releases/download/2.6.3/MiniZincIDE-2.6.3-bundled.dmg\nhdiutil attach minizinc.dmg\ncp -R /Volumes/MiniZinc*/MiniZincIDE.app minizinc_install/.\nexport PATH="$(pwd)/minizinc_install/MiniZincIDE.app/Contents/Resources":$PATH\nminizinc --version\n')])])]),t("h4",{attrs:{id:"windows-command-line"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#windows-command-line"}},[n._v("#")]),n._v(" Windows command line")]),n._v(" "),t("p",[n._v("Works on Windows Server 2022 with bash shell:")]),n._v(" "),t("div",{staticClass:"language- extra-class"},[t("pre",{pre:!0,attrs:{class:"language-text"}},[t("code",[n._v('mkdir minizinc_install\ncurl -o minizinc_setup.exe -L https://github.com/MiniZinc/MiniZincIDE/releases/download/2.6.3/MiniZincIDE-2.6.3-bundled-setup-win64.exe\ncmd //c "minizinc_setup.exe /verysilent /currentuser /norestart /suppressmsgboxes /sp"\nexport PATH="~/AppData/Local/Programs/MiniZinc":$PATH\nminizinc --version\n')])])]),t("h4",{attrs:{id:"skipping-minizinc-version-check"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#skipping-minizinc-version-check"}},[n._v("#")]),n._v(" Skipping minizinc version check")]),n._v(" "),t("p",[n._v("It may happen that you do not want to install minzinc.\nFor instance if you need to use only a part of the library which is not relying on minizinc at all,\nespecially when not relying on discrete-optimization which is the actual library depending on minizinc.")]),n._v(" "),t("p",[n._v("This can be troublesome as the minizinc binary version is checked by "),t("a",{attrs:{href:"https://airbus.github.io/discrete-optimization",target:"_blank",rel:"noopener noreferrer"}},[n._v("discrete-optimization"),t("OutboundLink")],1),n._v(" at library import.\nHowever discrete-optimization provides a way to bypass this check\nby setting the environment variable DO_SKIP_MZN_CHECK:")]),n._v(" "),t("div",{staticClass:"language-shell extra-class"},[t("pre",{pre:!0,attrs:{class:"language-shell"}},[t("code",[t("span",{pre:!0,attrs:{class:"token builtin class-name"}},[n._v("export")]),n._v(" "),t("span",{pre:!0,attrs:{class:"token assign-left variable"}},[n._v("DO_SKIP_MZN_CHECK")]),t("span",{pre:!0,attrs:{class:"token operator"}},[n._v("=")]),t("span",{pre:!0,attrs:{class:"token number"}},[n._v("1")]),n._v("\n")])])]),t("p",[n._v("Please note however that the library is never tested without minizinc (or minizinc versions < 2.6).")]),n._v(" "),t("h3",{attrs:{id:"python-3-8-environment"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#python-3-8-environment"}},[n._v("#")]),n._v(" Python 3.8+ environment")]),n._v(" "),t("p",[n._v("The use of a virtual environment for scikit-decide is recommended, and you will need to ensure that the environment use a Python version greater than 3.8.\nThis can be achieved either by using "),t("a",{attrs:{href:"https://docs.conda.io/en/latest/",target:"_blank",rel:"noopener noreferrer"}},[n._v("conda"),t("OutboundLink")],1),n._v(" or by using "),t("a",{attrs:{href:"https://github.com/pyenv/pyenv",target:"_blank",rel:"noopener noreferrer"}},[n._v("pyenv"),t("OutboundLink")],1),n._v(" (or "),t("a",{attrs:{href:"https://github.com/pyenv-win/pyenv-win",target:"_blank",rel:"noopener noreferrer"}},[n._v("pyenv-win"),t("OutboundLink")],1),n._v(" on windows)\nand "),t("a",{attrs:{href:"https://docs.python.org/fr/3/library/venv.html",target:"_blank",rel:"noopener noreferrer"}},[n._v("venv"),t("OutboundLink")],1),n._v(" module.")]),n._v(" "),t("p",[n._v("The following examples show how to create a virtual environment with Python version 3.8.13 with the mentioned methods.")]),n._v(" "),t("h4",{attrs:{id:"with-conda-all-platforms"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#with-conda-all-platforms"}},[n._v("#")]),n._v(" With conda (all platforms)")]),n._v(" "),t("div",{staticClass:"language-shell extra-class"},[t("pre",{pre:!0,attrs:{class:"language-shell"}},[t("code",[n._v("conda create -n skdecide "),t("span",{pre:!0,attrs:{class:"token assign-left variable"}},[n._v("python")]),t("span",{pre:!0,attrs:{class:"token operator"}},[n._v("=")]),t("span",{pre:!0,attrs:{class:"token number"}},[n._v("3.8")]),n._v(".13\nconda activate skdecide\n")])])]),t("h4",{attrs:{id:"with-pyenv-venv-linux-macos"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#with-pyenv-venv-linux-macos"}},[n._v("#")]),n._v(" With pyenv + venv (Linux/MacOS)")]),n._v(" "),t("div",{staticClass:"language-shell extra-class"},[t("pre",{pre:!0,attrs:{class:"language-shell"}},[t("code",[n._v("pyenv "),t("span",{pre:!0,attrs:{class:"token function"}},[n._v("install")]),n._v(" "),t("span",{pre:!0,attrs:{class:"token number"}},[n._v("3.8")]),n._v(".13\npyenv shell "),t("span",{pre:!0,attrs:{class:"token number"}},[n._v("3.8")]),n._v(".13\npython -m venv skdecide-venv\n"),t("span",{pre:!0,attrs:{class:"token builtin class-name"}},[n._v("source")]),n._v(" skdecide-venv/bin/activate\n")])])]),t("h4",{attrs:{id:"with-pyenv-win-venv-windows"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#with-pyenv-win-venv-windows"}},[n._v("#")]),n._v(" With pyenv-win + venv (Windows)")]),n._v(" "),t("div",{staticClass:"language-shell extra-class"},[t("pre",{pre:!0,attrs:{class:"language-shell"}},[t("code",[n._v("pyenv "),t("span",{pre:!0,attrs:{class:"token function"}},[n._v("install")]),n._v(" "),t("span",{pre:!0,attrs:{class:"token number"}},[n._v("3.8")]),n._v(".13\npyenv shell "),t("span",{pre:!0,attrs:{class:"token number"}},[n._v("3.8")]),n._v(".13\npython -m venv skdecide-venv\nskdecide-venv"),t("span",{pre:!0,attrs:{class:"token punctuation"}},[n._v("\\")]),n._v("Scripts"),t("span",{pre:!0,attrs:{class:"token punctuation"}},[n._v("\\")]),n._v("activate\n")])])]),t("h2",{attrs:{id:"install-scikit-decide-library"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#install-scikit-decide-library"}},[n._v("#")]),n._v(" Install scikit-decide library")]),n._v(" "),t("h3",{attrs:{id:"full-install-recommended"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#full-install-recommended"}},[n._v("#")]),n._v(" Full install [Recommended]")]),n._v(" "),t("p",[n._v("Install scikit-decide library from PyPI with all dependencies required by domains/solvers in the hub (scikit-decide catalog).")]),n._v(" "),t("div",{staticClass:"language-shell extra-class"},[t("pre",{pre:!0,attrs:{class:"language-shell"}},[t("code",[n._v("pip "),t("span",{pre:!0,attrs:{class:"token function"}},[n._v("install")]),n._v(" -U pip\npip "),t("span",{pre:!0,attrs:{class:"token function"}},[n._v("install")]),n._v(" -U scikit-decide"),t("span",{pre:!0,attrs:{class:"token punctuation"}},[n._v("[")]),n._v("all"),t("span",{pre:!0,attrs:{class:"token punctuation"}},[n._v("]")]),n._v("\n")])])]),t("h3",{attrs:{id:"minimal-install"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#minimal-install"}},[n._v("#")]),n._v(" Minimal install")]),n._v(" "),t("p",[n._v("Alternatively you can choose to only install the core library, which is enough if you intend to create your own domain and solver.")]),n._v(" "),t("div",{staticClass:"language-shell extra-class"},[t("pre",{pre:!0,attrs:{class:"language-shell"}},[t("code",[n._v("pip "),t("span",{pre:!0,attrs:{class:"token function"}},[n._v("install")]),n._v(" -U pip\npip "),t("span",{pre:!0,attrs:{class:"token function"}},[n._v("install")]),n._v(" -U scikit-decide\n")])])]),t("h2",{attrs:{id:"known-issues"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#known-issues"}},[n._v("#")]),n._v(" Known issues")]),n._v(" "),t("h3",{attrs:{id:"pygrib"}},[t("a",{staticClass:"header-anchor",attrs:{href:"#pygrib"}},[n._v("#")]),n._v(" pygrib")]),n._v(" "),t("p",[n._v("When installing "),t("a",{attrs:{href:"https://jswhit.github.io/pygrib/index.html",target:"_blank",rel:"noopener noreferrer"}},[n._v("pygrib"),t("OutboundLink")],1),n._v(" on MacOS ARM (in dependencies of "),t("code",[n._v("scikit-decide[all]")]),n._v("),\nno wheel exists on PyPI and there is issues when pip tries to build it.\nYou can overcome this by first installing "),t("code",[n._v("eccodes")]),n._v(" which provides GRIB header files required to build the "),t("code",[n._v("pygrib")]),n._v(" wheel:")]),n._v(" "),t("div",{staticClass:"language-shell extra-class"},[t("pre",{pre:!0,attrs:{class:"language-shell"}},[t("code",[n._v("brew "),t("span",{pre:!0,attrs:{class:"token function"}},[n._v("install")]),n._v(" eccodes\n")])])]),t("p",[n._v("Then, reinstall "),t("code",[n._v("scikit-decide[all]")]),n._v(" with pip.")]),n._v(" "),t("p",[n._v("If the issue persists, you can try to install the pygrib package available on conda-forge:")]),n._v(" "),t("div",{staticClass:"language-shell extra-class"},[t("pre",{pre:!0,attrs:{class:"language-shell"}},[t("code",[n._v("conda "),t("span",{pre:!0,attrs:{class:"token function"}},[n._v("install")]),n._v(" -c conda-forge pygrib\n")])])])])}),[],!1,null,null,null);e.default=a.exports}}]);