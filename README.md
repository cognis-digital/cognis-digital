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

**372 open-source tools · 55⭐ and climbing · 8 languages · every tool CLI + JSON/SARIF + MCP-native.** Single-purpose, self-hostable, and built to plug into your stack (SARIF→code-scanning, STIX/MISP/Sigma→SIEM, MCP→agents).

### 🏆 Most popular

| Tool | ⭐ | What it does |
|------|----|--------------|
| **[c2detect](https://github.com/cognis-digital/c2detect)** | 28 | C2 server fingerprinter |
| **[adsbwatch](https://github.com/cognis-digital/adsbwatch)** | 2 | Analyze an ADS-B feed/CSV for anomalies: callsign spoofing, squawk 7500/7600/7700,… |
| **[mcpharden](https://github.com/cognis-digital/mcpharden)** | 1 | MCP server hardening linter |
| **[maritimeint](https://github.com/cognis-digital/maritimeint)** | 1 | AIS vessel tracking & sanctions-evasion anomaly detection |
| **[compliance-atlas](https://github.com/cognis-digital/compliance-atlas)** | 1 | Condensed, cross-walked reference for SOC2, ISO 27001, NIST CSF/800-53/800-171,… |
| **[gsafinder](https://github.com/cognis-digital/gsafinder)** | 1 | GSA Schedule opportunity surveyor |

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

### 🔭 OSINT & Intelligence  <sub>(47 tools)</sub>

| Tool | ⭐ | What it does |
|------|----|--------------|
| **[adsbwatch](https://github.com/cognis-digital/adsbwatch)** | 2 | Analyze an ADS-B feed/CSV for anomalies: callsign spoofing, squawk 7500/7600/7700,… |
| **[blescope](https://github.com/cognis-digital/blescope)** | 1 | Sniff and decode BLE GATT traffic, fingerprint device profiles, and assert on… |
| **[comint-osquery](https://github.com/cognis-digital/comint-osquery)** | 1 | DISA STIG-aligned osquery configs + RMF mapper |
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
| **[c2detect](https://github.com/cognis-digital/c2detect)** | 28 | C2 server fingerprinter |
| **[modpot](https://github.com/cognis-digital/modpot)** | 1 | Spin up a high-interaction Modbus/DNP3 ICS honeypot that logs attacker register… |
| **[sbomb](https://github.com/cognis-digital/sbomb)** | 1 | Generate a CycloneDX SBOM directly from an unpacked firmware root filesystem and… |
| **[uefiscan](https://github.com/cognis-digital/uefiscan)** | 1 | Audit UEFI firmware dumps for missing Secure Boot keys, unsigned modules, S3… |
| **[yararun](https://github.com/cognis-digital/yararun)** | 1 | Run simple YARA-style string/regex rules over a directory |
| **[apidiff](https://github.com/cognis-digital/apidiff)** | — | Breaking-change detector for OpenAPI / GraphQL across commits |

### 🧰 Tools & Utilities  <sub>(141 tools)</sub>

| Tool | ⭐ | What it does |
|------|----|--------------|
| **[dealflow](https://github.com/cognis-digital/dealflow)** | 1 | Model your sales pipeline as a YAML state machine and compute conversion rates,… |
| **[otaverify](https://github.com/cognis-digital/otaverify)** | 1 | Validate OTA update packages end-to-end: signature chains, rollback protection,… |
| **[TGC](https://github.com/cognis-digital/TGC)** | — | — |
| **[accessreview](https://github.com/cognis-digital/accessreview)** | — | Periodic user-access-review (UAR) campaign runner |
| **[airgap-pkg](https://github.com/cognis-digital/airgap-pkg)** | — | Self-contained installer for airgapped (SIPR/JWICS-style) environments |
| **[alertmux](https://github.com/cognis-digital/alertmux)** | — | Alert dedup, correlation, and routing in front of Grafana / PagerDuty |

> Browse all **372** repos → [cognis-digital repositories](https://github.com/cognis-digital?tab=repositories) · filter by topic (`mcp-security`, `osint`, `detection-engineering`, `compliance`).
<!-- FEATURED:END -->

## 🌐 Languages across the suite

![Python](https://img.shields.io/badge/Python-3776AB?logo=python&logoColor=white) ![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?logo=javascript&logoColor=black) ![Go](https://img.shields.io/badge/Go-00ADD8?logo=go&logoColor=white) ![Rust](https://img.shields.io/badge/Rust-000000?logo=rust&logoColor=white) ![Shell](https://img.shields.io/badge/Shell-4EAA25?logo=gnubash&logoColor=white) ![PowerShell](https://img.shields.io/badge/PowerShell-5391FE?logo=powershell&logoColor=white) ![Solidity](https://img.shields.io/badge/Solidity-363636?logo=solidity&logoColor=white) ![Docker](https://img.shields.io/badge/Dockerfile-2496ED?logo=docker&logoColor=white)

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
