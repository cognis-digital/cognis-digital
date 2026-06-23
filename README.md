<a name="top"></a>
<div align="center">

<img src="https://capsule-render.vercel.app/api?type=waving&color=0:6b46c1,50:5a3fb0,100:2b6cb0&height=210&section=header&text=COGNIS%20%20DIGITAL&fontSize=66&fontColor=ffffff&fontAlignY=40&stroke=00000022" width="100%"/>

### Making Tomorrow Better Today

<a href="https://cognis.digital"><img src="https://img.shields.io/badge/Web-cognis.digital-6b46c1?style=for-the-badge&logo=google-chrome&logoColor=white"/></a>
<a href="https://github.com/cognis-digital/cognis-neural-suite"><img src="https://img.shields.io/badge/Tools-372%2B-2b6cb0?style=for-the-badge&logo=github&logoColor=white"/></a>
<img src="https://img.shields.io/badge/MCP-native-000000?style=for-the-badge"/>
<img src="https://img.shields.io/badge/License-COCL%201.0-2ea043?style=for-the-badge"/>

<img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=600&size=24&duration=3500&pause=1000&color=6B46C1&center=true&vCenter=true&width=940&height=50&lines=Advanced+Cybersecurity+%E2%80%94+AI+Innovation+%E2%80%94+Blockchain;The+Cognis+Neural+Suite%3A+372%2B+open-source+tools;Single-purpose+%C2%B7+Self-hostable+%C2%B7+MCP-native;Securing+the+agentic+era%2C+one+tool+at+a+time" width="940"/>

</div>

## Usage — step by step

This is the Cognis Digital org profile. The tools live in the [Cognis Neural Suite](https://github.com/cognis-digital/cognis-neural-suite) — here is how to go from catalog to a running tool.

1. **Browse the catalog** — open the [full catalog](https://github.com/cognis-digital/cognis-neural-suite) (or [awesome-cognis](https://github.com/cognis-digital/awesome-cognis)) and pick a tool.
2. **Install it** — most tools ship a CLI on PyPI under the `cognis-` prefix, e.g.:
   ```bash
   pip install cognis-mcpharden     # see the chosen tool's README for its real package name
   ```
3. **Run the primary command** and get machine-readable output — tools emit JSON/SARIF:
   ```bash
   mcpharden scan . --format sarif --out report.sarif
   ```
4. **Point your agents at it** — every tool ships an MCP server, so Claude Desktop / Cursor / Cognis.Studio can scan, audit, and remediate autonomously (run the tool's `mcp` command).
5. **Automate in CI** — gate builds on findings and upload SARIF to code scanning:
   ```yaml
   - run: pip install cognis-mcpharden
   - run: mcpharden scan . --format sarif --out report.sarif --fail-on high
   - uses: github/codeql-action/upload-sarif@v3
     with: { sarif_file: report.sarif }
   ```

## 👋 Welcome

**Cognis Digital** (Wyoming, USA) builds the **[Cognis Neural Suite](https://github.com/cognis-digital/cognis-neural-suite)** — **372+ single-purpose, self-hostable, MCP-native** tools across security, AI, military/IC, compliance, data, dev, and business. Every tool ships a CLI, JSON/SARIF output, an MCP server, polyglot ports (Py/JS/Go/Rust), a Dev Container, cross-OS + cloud deploy, and CI/CodeQL — and **most ship real, tested code**.

> 🧠 **Every tool is an MCP capability** — point your agents (Claude Desktop, Cursor, Cognis.Studio) at the suite and they can scan, audit, and remediate autonomously.

<div align="center">

<!-- reliable buttons (shields render even when stats services are rate-limited) -->
<a href="https://cognis.digital"><img src="https://img.shields.io/badge/%F0%9F%8C%90_Website-cognis.digital-6b46c1?style=for-the-badge"/></a>
<a href="https://github.com/cognis-digital/cognis-neural-suite"><img src="https://img.shields.io/badge/%F0%9F%97%82%EF%B8%8F_Catalog-372%2B_tools-2b6cb0?style=for-the-badge"/></a>
<a href="https://github.com/cognis-digital/cognis-code/discussions"><img src="https://img.shields.io/badge/%F0%9F%92%AC_Discussions-join-000000?style=for-the-badge"/></a>
<a href="mailto:licensing@cognis.digital"><img src="https://img.shields.io/badge/%F0%9F%92%BC_Commercial-licensing-2ea043?style=for-the-badge"/></a>
<img src="https://img.shields.io/github/followers/cognis-digital?style=for-the-badge&label=Follow&color=6b46c1&logo=github"/>

<br/><br/>

<!-- live stats (cached to dodge rate limits; shields above always render) -->
<img height="160" src="https://github-readme-stats.vercel.app/api?username=cognis-digital&show_icons=true&theme=tokyonight&hide_border=true&count_private=true&include_all_commits=true&cache_seconds=86400"/>
<img height="160" src="https://streak-stats.demolab.com/?user=cognis-digital&theme=tokyonight&hide_border=true"/>
<br/>
<img height="150" src="https://github-readme-stats.vercel.app/api/top-langs/?username=cognis-digital&layout=compact&theme=tokyonight&hide_border=true&langs_count=10&cache_seconds=86400"/>
<img height="150" src="https://github-profile-trophy.vercel.app/?username=cognis-digital&theme=algolia&no-frame=true&column=4&margin-w=4"/>

<br/>

<img src="https://komarev.com/ghpvc/?username=cognis-digital&style=for-the-badge&color=6b46c1&label=Profile+Views"/>

</div>

## 🚀 Explore the suite

**[🗂️ Full catalog → cognis-neural-suite](https://github.com/cognis-digital/cognis-neural-suite)** · [⭐ awesome-cognis](https://github.com/cognis-digital/awesome-cognis) · [🔗 cognis-sources](https://github.com/cognis-digital/cognis-sources)

<!-- FEATURED:START (generated by build_profile.py) -->
## 🚀 Featured tools

**372 open-source tools · 58⭐ and climbing · 8 languages · every tool CLI + JSON/SARIF + MCP-native.** Single-purpose, self-hostable, and built to plug into your stack (SARIF→code-scanning, STIX/MISP/Sigma→SIEM, MCP→agents).

### 🏆 Most popular

| Tool | ⭐ | What it does |
|------|----|--------------|
| **[c2detect](https://github.com/cognis-digital/c2detect)** | 29 | C2 server fingerprinter |
| **[comint-osquery](https://github.com/cognis-digital/comint-osquery)** | 2 | DISA STIG-aligned osquery configs + RMF mapper |
| **[adsbwatch](https://github.com/cognis-digital/adsbwatch)** | 2 | Analyze an ADS-B feed/CSV for anomalies: callsign spoofing, squawk 7500/7600/7700,… |
| **[keyhunt](https://github.com/cognis-digital/keyhunt)** | 1 | Scan firmware blobs and filesystem dumps for hardcoded private keys, API tokens,… |
| **[uefiscan](https://github.com/cognis-digital/uefiscan)** | 1 | Audit UEFI firmware dumps for missing Secure Boot keys, unsigned modules, S3… |
| **[fwxray](https://github.com/cognis-digital/fwxray)** | 1 | Diff two firmware images and surface exactly what changed: new binaries, flipped… |

### 📱 Mobile Security  <sub>(12 tools)</sub>

| Tool | ⭐ | What it does |
|------|----|--------------|
| **[apkpeek](https://github.com/cognis-digital/apkpeek)** | — | One-command static triage of Android APK/AAB binaries: surfaces hardcoded secrets,… |
| **[apkprobe](https://github.com/cognis-digital/apkprobe)** | — | Android APK static security analyzer |
| **[dastlite](https://github.com/cognis-digital/dastlite)** | — | A headless, config-as-code DAST runner that crawls an authenticated web/mobile-API… |
| **[deeplinkfuzz](https://github.com/cognis-digital/deeplinkfuzz)** | — | Fuzzes Android/iOS deep links, intents, and custom URL schemes against an… |
| **[dvmobile](https://github.com/cognis-digital/dvmobile)** | — | Damn Vulnerable Mobile |
| **[hookcraft](https://github.com/cognis-digital/hookcraft)** | — | Generates ready-to-run Frida instrumentation scripts from a YAML intent (e.g |

### 🤖 AI · Agents · MCP  <sub>(66 tools)</sub>

| Tool | ⭐ | What it does |
|------|----|--------------|
| **[hermes](https://github.com/cognis-digital/hermes)** | 1 | Model-agnostic, portable long-term memory framework for AI agents (MCP-native) |
| **[keyhunt](https://github.com/cognis-digital/keyhunt)** | 1 | Scan firmware blobs and filesystem dumps for hardcoded private keys, API tokens,… |
| **[mcpharden](https://github.com/cognis-digital/mcpharden)** | 1 | MCP server hardening linter |
| **[tokenmeter](https://github.com/cognis-digital/tokenmeter)** | 1 | Token and cost counter / budgeter for LLM apps, CI-ready |
| **[uncensored-fleet](https://github.com/cognis-digital/uncensored-fleet)** | 1 | Deploy a local multi-model LLM fleet (llama.cpp) with an agent harness, hermes… |
| **[adversa](https://github.com/cognis-digital/adversa)** | — | LLM red-team harness |

### 🔭 OSINT & Intelligence  <sub>(48 tools)</sub>

| Tool | ⭐ | What it does |
|------|----|--------------|
| **[adsbwatch](https://github.com/cognis-digital/adsbwatch)** | 2 | Analyze an ADS-B feed/CSV for anomalies: callsign spoofing, squawk 7500/7600/7700,… |
| **[comint-osquery](https://github.com/cognis-digital/comint-osquery)** | 2 | DISA STIG-aligned osquery configs + RMF mapper |
| **[blescope](https://github.com/cognis-digital/blescope)** | 1 | Sniff and decode BLE GATT traffic, fingerprint device profiles, and assert on… |
| **[conflictwatch](https://github.com/cognis-digital/conflictwatch)** | 1 | Open-source conflict monitoring & situational awareness |
| **[geolens](https://github.com/cognis-digital/geolens)** | 1 | Image geolocation toolkit |
| **[maritimeint](https://github.com/cognis-digital/maritimeint)** | 1 | AIS vessel tracking & sanctions-evasion anomaly detection |

### 📋 Compliance & GRC  <sub>(28 tools)</sub>

| Tool | ⭐ | What it does |
|------|----|--------------|
| **[compliance-atlas](https://github.com/cognis-digital/compliance-atlas)** | 1 | Condensed, cross-walked reference for SOC2, ISO 27001, NIST CSF/800-53/800-171,… |
| **[deidproof](https://github.com/cognis-digital/deidproof)** | 1 | Re-identification risk assessment that computes k-anonymity, l-diversity, and HIPAA… |
| **[fedramplens](https://github.com/cognis-digital/fedramplens)** | 1 | FedRAMP boundary visualizer & OSCAL-format SSP/POAM generator |
| **[gsafinder](https://github.com/cognis-digital/gsafinder)** | 1 | GSA Schedule opportunity surveyor |
| **[stigsentry](https://github.com/cognis-digital/stigsentry)** | 1 | DISA STIG checker + NIST 800-53 RMF mapper + POAM emitter |
| **[admitd](https://github.com/cognis-digital/admitd)** | — | Kubernetes policy-as-code admission engine |

### 🔬 Forensics & Data  <sub>(9 tools)</sub>

| Tool | ⭐ | What it does |
|------|----|--------------|
| **[canzap](https://github.com/cognis-digital/canzap)** | 1 | Replay, fuzz, and assert on CAN bus traffic from a .pcap or SocketCAN interface… |
| **[fwxray](https://github.com/cognis-digital/fwxray)** | 1 | Diff two firmware images and surface exactly what changed: new binaries, flipped… |
| **[pcapsummary](https://github.com/cognis-digital/pcapsummary)** | 1 | Summarize flows/talkers/protocols from a pcap text export |
| **[browserforensics](https://github.com/cognis-digital/browserforensics)** | — | Analyze exported browser history/downloads for IOCs and exfil signs |
| **[entropyscan](https://github.com/cognis-digital/entropyscan)** | — | Scan files and binaries for high-entropy regions to flag packing, encryption, and… |
| **[exfilwatch](https://github.com/cognis-digital/exfilwatch)** | — | Detect DNS/HTTP exfiltration patterns (entropy, beaconing) in logs |

### ☁️ Infra · Cloud · Mesh  <sub>(27 tools)</sub>

| Tool | ⭐ | What it does |
|------|----|--------------|
| **[crackq](https://github.com/cognis-digital/crackq)** | 1 | Self-hosted password cracking queue |
| **[awesome-cognis](https://github.com/cognis-digital/awesome-cognis)** | — | Awesome Cognis |
| **[bootkit](https://github.com/cognis-digital/bootkit)** | — | Air-gapped cluster bootstrap planner |
| **[cloud-setups](https://github.com/cognis-digital/cloud-setups)** | — | Firebase, GCP, and Azure project setups |
| **[cloudbill](https://github.com/cognis-digital/cloudbill)** | — | Multi-cloud cost report, anomaly detection, and FOCUS export |
| **[cloudkeys](https://github.com/cognis-digital/cloudkeys)** | — | Find leaked cloud keys (AWS/GCP/Azure) + classify blast radius |

### 🛡️ Security & Detection  <sub>(42 tools)</sub>

| Tool | ⭐ | What it does |
|------|----|--------------|
| **[c2detect](https://github.com/cognis-digital/c2detect)** | 29 | C2 server fingerprinter |
| **[modpot](https://github.com/cognis-digital/modpot)** | 1 | Spin up a high-interaction Modbus/DNP3 ICS honeypot that logs attacker register… |
| **[sbomb](https://github.com/cognis-digital/sbomb)** | 1 | Generate a CycloneDX SBOM directly from an unpacked firmware root filesystem and… |
| **[uefiscan](https://github.com/cognis-digital/uefiscan)** | 1 | Audit UEFI firmware dumps for missing Secure Boot keys, unsigned modules, S3… |
| **[yararun](https://github.com/cognis-digital/yararun)** | 1 | Run simple YARA-style string/regex rules over a directory |
| **[apidiff](https://github.com/cognis-digital/apidiff)** | — | Breaking-change detector for OpenAPI / GraphQL across commits |

### 🧰 Tools & Utilities  <sub>(140 tools)</sub>

| Tool | ⭐ | What it does |
|------|----|--------------|
| **[dealflow](https://github.com/cognis-digital/dealflow)** | 1 | Model your sales pipeline as a YAML state machine and compute conversion rates,… |
| **[frontline-drones](https://github.com/cognis-digital/frontline-drones)** | 1 | Descriptive, citation-grade catalog of frontline & commercial drones + the open… |
| **[otaverify](https://github.com/cognis-digital/otaverify)** | 1 | Validate OTA update packages end-to-end: signature chains, rollback protection,… |
| **[accessreview](https://github.com/cognis-digital/accessreview)** | — | Periodic user-access-review (UAR) campaign runner |
| **[airgap-pkg](https://github.com/cognis-digital/airgap-pkg)** | — | Self-contained installer for airgapped (SIPR/JWICS-style) environments |
| **[alertmux](https://github.com/cognis-digital/alertmux)** | — | Alert dedup, correlation, and routing in front of Grafana / PagerDuty |

> Browse all **372** repos → [cognis-digital repositories](https://github.com/cognis-digital?tab=repositories) · filter by topic (`mcp-security`, `osint`, `detection-engineering`, `compliance`).
<!-- FEATURED:END -->

<!-- HIGHLIGHTS:START -->
## ⚡ Recent upgrades (June 2026)

The suite just got a major capability + quality pass — additive across the catalog:

- **Real intelligence feeds, edge/air-gap ready** — 35 keyless sources (CISA KEV, EPSS, OSV, NVD, MITRE ATT&CK STIX, NIST OSCAL 800-53, abuse.ch C2/IOC, OFAC, GDELT, OpenSky, USGS, Wikimedia) wired into the tools via a stdlib fetch→cache→**offline**→snapshot module.
- **262,351-vulnerability offline DB** bundled into the vulnerability scanners — real OSV records (CVE/GHSA aliases, CVSS, affected packages) queryable with zero network.
- **Standards exports everywhere** — SARIF (code-scanning), STIX 2.1, OSCAL, Sigma + Suricata, GeoJSON/KML, CSV across the suite.
- **Deeper detection** — C2 campaign correlation, MCP fleet-posture + supply-chain (OWASP Agentic Top-10 2026), maritime track-interaction (CPA/TCPA), and more — each with expanded test suites and candid docs.
- **Passive + authorization-gated active scanning** and polyglot ports rolling out across the scanners.

> Every tool stays single-purpose, self-hostable, **MCP-native**, and defensively-scoped.
<!-- HIGHLIGHTS:END -->

## 🌐 Languages across the suite

**Polyglot by design.** The suite is Python-first but ports outward so a tool exists in the language of *your* deployment target — mainframe to mobile, kernel to contract. Goal: reach every major language. Below, **● live in-repo** today, **○ rolling out** (tracked in [`polyglot-roadmap`](https://github.com/cognis-digital/awesome-cognis)).

**● Shipping now**
![Python](https://img.shields.io/badge/Python-3776AB?logo=python&logoColor=white) ![TypeScript](https://img.shields.io/badge/TypeScript-3178C6?logo=typescript&logoColor=white) ![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?logo=javascript&logoColor=black) ![Go](https://img.shields.io/badge/Go-00ADD8?logo=go&logoColor=white) ![Rust](https://img.shields.io/badge/Rust-000000?logo=rust&logoColor=white) ![C%23](https://img.shields.io/badge/C%23-512BD4?logo=dotnet&logoColor=white) ![Ruby](https://img.shields.io/badge/Ruby-CC342D?logo=ruby&logoColor=white) ![Lua](https://img.shields.io/badge/Lua-2C2D72?logo=lua&logoColor=white) ![C](https://img.shields.io/badge/C-A8B9CC?logo=c&logoColor=black) ![COBOL](https://img.shields.io/badge/COBOL-005CA5?logo=gnu&logoColor=white) ![Perl](https://img.shields.io/badge/Perl-39457E?logo=perl&logoColor=white) ![Shell](https://img.shields.io/badge/Shell-4EAA25?logo=gnubash&logoColor=white) ![PowerShell](https://img.shields.io/badge/PowerShell-5391FE?logo=powershell&logoColor=white)

**📱 Mobile — languages & frameworks**
![Swift](https://img.shields.io/badge/Swift-FA7343?logo=swift&logoColor=white) ![Kotlin](https://img.shields.io/badge/Kotlin-7F52FF?logo=kotlin&logoColor=white) ![Java](https://img.shields.io/badge/Java-007396?logo=openjdk&logoColor=white) ![Objective-C](https://img.shields.io/badge/Objective--C-438EFF?logo=apple&logoColor=white) ![Dart](https://img.shields.io/badge/Dart-0175C2?logo=dart&logoColor=white) ![Flutter](https://img.shields.io/badge/Flutter-02569B?logo=flutter&logoColor=white) ![SwiftUI](https://img.shields.io/badge/SwiftUI-0071E3?logo=swift&logoColor=white) ![Jetpack%20Compose](https://img.shields.io/badge/Jetpack_Compose-4285F4?logo=jetpackcompose&logoColor=white) ![React Native](https://img.shields.io/badge/React_Native-61DAFB?logo=react&logoColor=black)

**⛓️ Smart-contract / Web3** — audited in production across EVM · Base · Arbitrum · Blast · Polygon · Solana · Algorand · Nervos · Cronos · Conflux · XRPL
![Solidity](https://img.shields.io/badge/Solidity-363636?logo=solidity&logoColor=white) ![Vyper](https://img.shields.io/badge/Vyper-2980B9?logo=python&logoColor=white) ![Yul](https://img.shields.io/badge/Yul-4E5D94?logo=ethereum&logoColor=white) ![Huff](https://img.shields.io/badge/Huff-1A1A1A?logo=ethereum&logoColor=white) ![Move](https://img.shields.io/badge/Move_(Aptos%2FSui)-4A5568?logo=aptos&logoColor=white) ![Cairo](https://img.shields.io/badge/Cairo_(Starknet)-EC796B?logo=starknet&logoColor=white) ![Rust](https://img.shields.io/badge/Rust_(Anchor%2FSolana)-000000?logo=rust&logoColor=white) ![Sway](https://img.shields.io/badge/Sway_(Fuel)-00F58C?logo=data:image/svg+xml;base64,&logoColor=black) ![Clarity](https://img.shields.io/badge/Clarity_(Stacks)-5546FF?logo=stacks&logoColor=white) ![FunC](https://img.shields.io/badge/FunC%2FTact_(TON)-0098EA?logo=tonramp&logoColor=white) ![TEAL](https://img.shields.io/badge/TEAL%2FPyTeal_(Algorand)-000000?logo=algorand&logoColor=white) ![Plutus](https://img.shields.io/badge/Plutus_(Cardano)-0033AD?logo=cardano&logoColor=white) ![ink!](https://img.shields.io/badge/ink!_(Polkadot)-E6007A?logo=polkadot&logoColor=white) ![Michelson](https://img.shields.io/badge/Michelson%2FLIGO_(Tezos)-2C7DF7?logo=tezos&logoColor=white) ![Scilla](https://img.shields.io/badge/Scilla_(Zilliqa)-49C1BF&logoColor=white)

**🕵️ Zero-knowledge / circuits** (trending) — ![Noir](https://img.shields.io/badge/Noir_(Aztec)-2D2D2D&logoColor=white) ![Circom](https://img.shields.io/badge/Circom-1C1C1C&logoColor=white) ![Cairo-ZK](https://img.shields.io/badge/Cairo-EC796B?logo=starknet&logoColor=white) — auditing & formal-verification tooling (Slither/Foundry/Echidna/Mythril lineage) ports to each VM.

**⛓️ Chains we ship & audit on**
![EVM](https://img.shields.io/badge/EVM-3C3C3D?logo=ethereum&logoColor=white) ![Base](https://img.shields.io/badge/Base-0052FF?logo=coinbase&logoColor=white) ![Arbitrum](https://img.shields.io/badge/Arbitrum-28A0F0?logo=arbitrum&logoColor=white) ![Blast](https://img.shields.io/badge/Blast-FCFC03&logoColor=black) ![Polygon](https://img.shields.io/badge/Polygon-7B3FE4?logo=polygon&logoColor=white) ![Optimism](https://img.shields.io/badge/Optimism-FF0420?logo=optimism&logoColor=white) ![Solana](https://img.shields.io/badge/Solana-9945FF?logo=solana&logoColor=white) ![Algorand](https://img.shields.io/badge/Algorand-000000?logo=algorand&logoColor=white) ![Nervos](https://img.shields.io/badge/Nervos_CKB-3CC68A&logoColor=white) ![Cronos](https://img.shields.io/badge/Cronos-002D74?logo=cronos&logoColor=white) ![Conflux](https://img.shields.io/badge/Conflux-1A1A1A&logoColor=white) ![XRPL](https://img.shields.io/badge/XRPL-25A768?logo=xrp&logoColor=white) ![TON](https://img.shields.io/badge/TON-0098EA?logo=ton&logoColor=white) ![Aptos](https://img.shields.io/badge/Aptos-000000?logo=aptos&logoColor=white) ![Sui](https://img.shields.io/badge/Sui-4DA2FF?logo=sui&logoColor=white) ![Starknet](https://img.shields.io/badge/Starknet-EC796B?logo=starknet&logoColor=white)

**○ Rolling out** — systems, JVM, .NET, scripting, functional, data & legacy
![C++](https://img.shields.io/badge/C++-00599C?logo=cplusplus&logoColor=white) ![Zig](https://img.shields.io/badge/Zig-F7A41D?logo=zig&logoColor=black) ![Scala](https://img.shields.io/badge/Scala-DC322F?logo=scala&logoColor=white) ![Groovy](https://img.shields.io/badge/Groovy-4298B8?logo=apachegroovy&logoColor=white) ![F%23](https://img.shields.io/badge/F%23-378BBA?logo=dotnet&logoColor=white) ![VB.NET](https://img.shields.io/badge/VB.NET-512BD4?logo=dotnet&logoColor=white) ![PHP](https://img.shields.io/badge/PHP-777BB4?logo=php&logoColor=white) ![Elixir](https://img.shields.io/badge/Elixir-4B275F?logo=elixir&logoColor=white) ![Haskell](https://img.shields.io/badge/Haskell-5D4F85?logo=haskell&logoColor=white) ![Clojure](https://img.shields.io/badge/Clojure-5881D8?logo=clojure&logoColor=white) ![OCaml](https://img.shields.io/badge/OCaml-EC6813?logo=ocaml&logoColor=white) ![SQL](https://img.shields.io/badge/SQL-336791?logo=postgresql&logoColor=white) ![R](https://img.shields.io/badge/R-276DC3?logo=r&logoColor=white) ![Julia](https://img.shields.io/badge/Julia-9558B2?logo=julia&logoColor=white) ![Fortran](https://img.shields.io/badge/Fortran-734F96?logo=fortran&logoColor=white) ![Ada](https://img.shields.io/badge/Ada-2C2C2C?logo=gnu&logoColor=white) ![WebAssembly](https://img.shields.io/badge/WebAssembly-654FF0?logo=webassembly&logoColor=white)

<sub>Every port keeps the suite contract: a CLI, structured output (JSON/SARIF), and an MCP server. New-language tools land with a CI workflow that compiles & tests on the matching runner (GnuCOBOL, gcc, JDK, Swift, .NET, Ruby, Lua, Dart…).</sub>

## 👤 Founder & engineering stack

Led by **Christopher Hyatt** — Software & AI Engineer, founder of Cognis Digital, smart-contract auditor (Entersoft, intern→lead), and federal cybersecurity SME. 🛰️ **Hack-A-Sat 7th worldwide / 3,600+ teams** · **Top 1% TryHackMe** · Eagle Scout · CompTIA Security+ · CISSP Prep · AWS DevOps.

**Languages** · Python · TypeScript/JavaScript · Solidity · Rust · Go · Move · Cairo · Vyper · SQL · Bash
**AI / Agents** · self-hosted LLMs · RAG · evals & guardrails · MCP · Claude Agent SDK · LangGraph · CrewAI · AutoGen · LangChain · LlamaIndex · Ollama · vLLM · llama.cpp
**Chains** · EVM · Base · Arbitrum · Blast · Polygon · Solana · Algorand · Nervos · Cronos · Conflux · XRPL
**Engineering** · React/Next.js · FastAPI · Node.js · Docker · Kubernetes · AWS · Azure · GCP · PostgreSQL · Redis · Supabase · CI/CD
**Security & Web3** · smart-contract audit · formal verification · pentesting · Foundry · Slither · Echidna · Mythril · Burp Suite · Nmap · Kali · SIEM/Splunk · NIST 800-53 · MITRE ATT&CK
**Intelligence** · OSINT · SIGINT · GEOINT · HUMINT · ADINT &nbsp;|&nbsp; **PQC** · post-quantum crypto

<sub>Open to roles & contract work via <a href="https://devpairer.com">DevPairer</a>.</sub>

## 🤝 Get involved

⭐ Star the tools you use · 🛠️ Contribute under the collaboration-pull model (see any repo's `CONTRIBUTING.md`) · 🏢 Commercial use → `licensing@cognis.digital`

<div align="center"><img src="https://capsule-render.vercel.app/api?type=waving&color=0:2b6cb0,100:6b46c1&height=120&section=footer"/></div>

## Interoperability

`cognis-digital` composes with the 300+ tool Cognis suite — JSON in/out and a shared
OpenAI-compatible `/v1` backbone. See **[INTEROP.md](INTEROP.md)** for the
suite map, composition patterns, and reference stacks.

## Integrations

Forward `cognis-digital`'s findings to STIX/MISP/Sigma/Splunk/Elastic/Slack/webhooks via
[`cognis-connect`](https://github.com/cognis-digital/cognis-connect). See **[INTEGRATIONS.md](INTEGRATIONS.md)**.
