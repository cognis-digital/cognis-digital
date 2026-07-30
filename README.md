<a name="top"></a>
<div align="center">

<img src="https://capsule-render.vercel.app/api?type=waving&color=0:6b46c1,50:5a3fb0,100:2b6cb0&height=210&section=header&text=COGNIS%20%20DIGITAL&fontSize=66&fontColor=ffffff&fontAlignY=40&stroke=00000022" width="100%"/>

### A private engineering practice — innovating on behalf of the American citizen

<a href="https://cognis.digital"><img src="https://img.shields.io/badge/Web-cognis.digital-6b46c1?style=for-the-badge&logo=google-chrome&logoColor=white"/></a>
<img src="https://img.shields.io/badge/Made%20in-USA%20%F0%9F%87%BA%F0%9F%87%B8-2b6cb0?style=for-the-badge"/>
<img src="https://img.shields.io/badge/Engagements-by%20referral-000000?style=for-the-badge"/>
<img src="https://img.shields.io/badge/MCP-native-2ea043?style=for-the-badge"/>

<img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=600&size=23&duration=3600&pause=1000&color=6B46C1&center=true&vCenter=true&width=940&height=50&lines=Hard-target+security+where+SaaS+doesn't+reach;Firmware+%C2%B7+ICS%2FOT+%C2%B7+RF+%C2%B7+C2+%C2%B7+DFIR+%C2%B7+OSINT+%C2%B7+compliance;Single-purpose+%C2%B7+Self-hostable+%C2%B7+MCP-native;Built+in+the+USA%2C+for+the+people+who+defend+it" width="940"/>

</div>

<!-- cognis:stats:start -->

<div align="center">

<img src="https://img.shields.io/badge/Tools-397%2B-2b6cb0?style=for-the-badge&logo=github&logoColor=white"/>
<img src="https://img.shields.io/badge/Stars-93-6b46c1?style=for-the-badge&logo=github&logoColor=white"/>
<img src="https://img.shields.io/github/followers/cognis-digital?style=for-the-badge&label=Followers&color=5a3fb0&logo=github"/>
<a href="https://github.com/cognis-digital/c2detect"><img src="https://img.shields.io/badge/Flagship-c2detect%20%E2%AD%9033-c0392b?style=for-the-badge"/></a>

**397** self-hostable, MCP-native tools · **93** stars · flagship **[c2detect](https://github.com/cognis-digital/c2detect)** (⭐33)

</div>

<!-- cognis:stats:end -->

## 👋 Who we are

**Cognis Digital** (Wyoming, USA) is a **private engineering practice**. We work quietly, by referral, on the hardest problems in security and autonomy — and we publish the tools that come out of that work as open source, so the operators and analysts who need them can self-host them, air-gapped, with no vendor in the loop.

We don't run ads and we don't cold-sell. The work is the marketing: single-purpose, **self-hostable**, **MCP-native** tools that reach where mainstream and SaaS security stop — **firmware, ICS/OT, RF, C2, DFIR, OSINT, and compliance-as-code**. Every tool ships a CLI, machine-readable **JSON/SARIF** output, and an MCP server so your agents can scan, audit, and remediate autonomously.

> 🇺🇸 **USA-only, mission-first.** We build on behalf of the American citizen — the defenders, the operators, and the small teams holding the line.

<!-- cognis:featured:start -->

## ⚡ Start here — hard-target tools that reach where SaaS doesn't

**Firmware · ICS/OT · RF · C2 · DFIR · OSINT · compliance-as-code.** Every tool is single-purpose, **self-hostable**, and emits machine-readable JSON/SARIF. Counts below are live from the GitHub API.

### 🔴 Threat detection, C2 &amp; DFIR

| Tool | What it does |
| --- | --- |
| **[c2detect](https://github.com/cognis-digital/c2detect)** ⭐33 | C2 server fingerprinter — Cobalt Strike, Sliver, Mythic, Havoc, Brute Ratel |
| **[packpeek](https://github.com/cognis-digital/packpeek)** ⭐2 | Static packer/loader fingerprinter (C) — UPX/ASPack/Themida/MPRESS/VMProtect + entropy; emits YARA + SARIF. JSON out, CI-tested. |
| **[yararun](https://github.com/cognis-digital/yararun)** ⭐1 | Run simple YARA-style string/regex rules over a directory |

### 🔧 Firmware · hardware · ICS/OT · RF

| Tool | What it does |
| --- | --- |
| **[canzap](https://github.com/cognis-digital/canzap)** ⭐2 | Replay, fuzz, and assert on CAN bus traffic from a .pcap or SocketCAN interface with a tiny YAML DSL. |
| **[otaverify](https://github.com/cognis-digital/otaverify)** ⭐1 | Validate OTA update packages end-to-end: signature chains, rollback protection, anti-downgrade counters, and delta-patch integrity. |
| **[blescope](https://github.com/cognis-digital/blescope)** ⭐1 | Sniff and decode BLE GATT traffic, fingerprint device profiles, and assert on insecure pairing/characteristics in CI against a capture. |
| **[keyhound](https://github.com/cognis-digital/keyhound)** ⭐1 | Scan firmware blobs and filesystem dumps for hardcoded private keys, API tokens, default creds, and weak RSA/ECC material. |
| **[modlure](https://github.com/cognis-digital/modlure)** ⭐1 | Spin up a high-interaction Modbus/DNP3 ICS honeypot that logs attacker register reads/writes as structured JSON. |
| **[bootwarden](https://github.com/cognis-digital/bootwarden)** ⭐1 | Audit UEFI firmware dumps for missing Secure Boot keys, unsigned modules, S3 boot-script vulns, and known SMM threats. |

### 🪖 Defense · national security · geoint

| Tool | What it does |
| --- | --- |
| **[awesome-drone-warfare-osint](https://github.com/cognis-digital/awesome-drone-warfare-osint)** ⭐3 | Citation-grade OSINT dataset: 8,300+ foreign components across 195+ drone & missile platforms, with cited effectiveness/EW/counter-UAS statistics. MIT (code) / CC BY 4.0 (data). |
| **[spoofwatch](https://github.com/cognis-digital/spoofwatch)** ⭐2 | Detect & map GPS/GNSS jamming & spoofing from ADS-B/AIS position feeds — zero-dependency, offline. Cognis Digital. |
| **[adsbwatch](https://github.com/cognis-digital/adsbwatch)** ⭐2 | Analyze an ADS-B feed/CSV for anomalies: callsign spoofing, squawk 7500/7600/7700, and unusual loiter patterns. |
| **[scryer](https://github.com/cognis-digital/scryer)** ⭐1 | Multi-domain ISR sensor fusion for counternarcotics (non-kinetic) — EO/IR+radar+AIS+ADS-B track fusion, dark-contact cross-cue, coverage cost modeling, GeoJSON. Self-hosted, verified metrics. |
| **[frontline-drones](https://github.com/cognis-digital/frontline-drones)** ⭐1 | Descriptive, citation-grade catalog of frontline & commercial drones + the open autonomy ecosystem (PX4/ArduPilot/MAVLink) and NVIDIA's open Hugging Face robotics/perception models. MIT (code) / CC BY 4.0 (data). |

### 🛰️ OSINT &amp; all-source intelligence

| Tool | What it does |
| --- | --- |
| **[cryptotrace](https://github.com/cognis-digital/cryptotrace)** ⭐2 | Free-tier blockchain investigator — ETH/BTC clustering + sanctions xref |
| **[maritimeint](https://github.com/cognis-digital/maritimeint)** ⭐2 | AIS vessel tracking & sanctions-evasion anomaly detection |
| **[conflictwatch](https://github.com/cognis-digital/conflictwatch)** ⭐1 | Open-source conflict monitoring & situational awareness — ACLED/GDELT/UCDP + OSINT feeds, hotspots/trends, and a sourced 'what's working' lessons KB (OSINT/force-protection) |
| **[personagraph](https://github.com/cognis-digital/personagraph)** ⭐1 | Identity resolution dossier — username/email/phone cross-platform |

### 📋 Compliance-as-code &amp; GRC

| Tool | What it does |
| --- | --- |
| **[comint-osquery](https://github.com/cognis-digital/comint-osquery)** ⭐2 | DISA STIG-aligned osquery configs + RMF mapper |
| **[compliance-atlas](https://github.com/cognis-digital/compliance-atlas)** ⭐2 | Condensed, cross-walked reference for SOC2, ISO 27001, NIST CSF/800-53/800-171, CMMC, GDPR, CCPA, HIPAA, PCI DSS, EU AI Act |
| **[grcforge](https://github.com/cognis-digital/grcforge)** ⭐1 | GRC control crosswalk engine (NIST 800-53 / CIS / SOC 2) + gap analysis |
| **[oscalkit](https://github.com/cognis-digital/oscalkit)** ⭐1 | OSCAL compliance-as-code — validate, convert & diff control coverage for catalogs, profiles, component definitions & SSPs |
| **[stigsentry](https://github.com/cognis-digital/stigsentry)** ⭐1 | DISA STIG checker + NIST 800-53 RMF mapper + POAM emitter |
| **[deidproof](https://github.com/cognis-digital/deidproof)** ⭐1 | Re-identification risk assessment that computes k-anonymity, l-diversity, and HIPAA Safe Harbor compliance on a dataset. |

### 📱 Mobile &amp; application security

| Tool | What it does |
| --- | --- |
| **[rootsentry](https://github.com/cognis-digital/rootsentry)** ⭐1 | Mobile runtime-integrity detection: root/jailbreak/emulator/hook/tamper indicators with a scored posture verdict (RASP-style, zero deps). |
| **[apkprobe](https://github.com/cognis-digital/apkprobe)** ⭐1 | Android APK static security analyzer — MASTG-aligned, from-scratch binary-AXML decoder, zero dependencies. |

### 🤖 Agent, MCP &amp; AI security

| Tool | What it does |
| --- | --- |
| **[codegraph-mcp](https://github.com/cognis-digital/codegraph-mcp)** ⭐7 | No-train, on-prem code knowledge graph served to AI agents over MCP, with a hash-chained audit row for every read. |
| **[uncensored-fleet](https://github.com/cognis-digital/uncensored-fleet)** ⭐2 | Deploy a local multi-model LLM fleet (llama.cpp) with an agent harness, hermes memory, and a one-command CLI |

<!-- cognis:featured:end -->

<!-- cognis:frontier:start -->

## 🌱 Frontier — newly shipped, help them grow

Our newest work, where the stars haven't caught up to the engineering yet. If one earns a place in your stack, a ⭐ tells us to push it further.

| Tool | What it does |
| --- | --- |
| **[repolens](https://github.com/cognis-digital/repolens)** | Deterministic, token-budgeted, AST-aware repository context packs for LLM agents — with a hash-chained provenance row for every read. Zero-dependency CLI + MCP. |
| **[garrison](https://github.com/cognis-digital/garrison)** | Self-hosted cyber-ops training range & curriculum — role tracks, offline auto-grading, readiness scoring. SDK + one-line install. Cognis Digital. |
| **[obol](https://github.com/cognis-digital/obol)** | A file-based, chain-agnostic payment protocol for autonomous agents. |
| **[hazardwatch](https://github.com/cognis-digital/hazardwatch)** | Self-updating public-safety hazard monitor — USGS quakes + NASA fires/storms + NWS alerts, one map. Keyless, offline, auto-refreshing. Cognis Digital. |
| **[spoofwatch](https://github.com/cognis-digital/spoofwatch)** ⭐2 | Detect & map GPS/GNSS jamming & spoofing from ADS-B/AIS position feeds — zero-dependency, offline. Cognis Digital. |
| **[plumewatch](https://github.com/cognis-digital/plumewatch)** | Detect & track smoke plumes in satellite/aerial/photo imagery — classical CV, zero-dependency, offline. Cognis Digital. |
| **[taskloom](https://github.com/cognis-digital/taskloom)** | Deterministic, auditable multi-agent orchestration — register tools, run a fixed or rule-driven plan, get a fully-traced reproducible result. Zero deps, offline. |
| **[fixpoint](https://github.com/cognis-digital/fixpoint)** | Empirically characterizing convergence of the AI code generate-verify-repair loop: converge / stall / oscillate / exhaust. Deterministic CI study + data-only 43-task benchmark. |

> **Renamed to single-word brands** (old links now redirect here): `uefiscan` → **[bootwarden](https://github.com/cognis-digital/bootwarden)** · `keyhunt` → **[keyhound](https://github.com/cognis-digital/keyhound)** · `modpot` → **[modlure](https://github.com/cognis-digital/modlure)** — among others in the rebrand.

<!-- cognis:frontier:end -->

## ⚙️ Usage — catalog to running tool

The tools live in the [Cognis Neural Suite](https://github.com/cognis-digital/cognis-neural-suite). Going from catalog to a running tool:

1. **Pick a tool** — browse the [full catalog](https://github.com/cognis-digital/cognis-neural-suite) or the featured tables above.
2. **Install it** — most tools ship a CLI on PyPI under the `cognis-` prefix (see the tool's README for its exact package name):
   ```bash
   pip install cognis-mcpharden
   ```
3. **Run it — machine-readable by default** (JSON/SARIF):
   ```bash
   mcpharden scan . --format sarif --out report.sarif
   ```
4. **Point your agents at it** — every tool ships an MCP server, so Claude Desktop / Cursor / Cognis.Studio can drive it autonomously (run the tool's `mcp` command).
5. **Automate in CI** — gate builds on findings and upload SARIF to code scanning:
   ```yaml
   - run: pip install cognis-mcpharden
   - run: mcpharden scan . --format sarif --out report.sarif --fail-on high
   - uses: github/codeql-action/upload-sarif@v3
     with: { sarif_file: report.sarif }
   ```

<!-- HIGHLIGHTS:START -->
## ⚡ Recent upgrades

The suite got a major capability + quality pass — additive across the catalog:

- **Real intelligence feeds, edge/air-gap ready** — 35 keyless sources (CISA KEV, EPSS, OSV, NVD, MITRE ATT&CK STIX, NIST OSCAL 800-53, abuse.ch C2/IOC, OFAC, GDELT, OpenSky, USGS, Wikimedia) wired into the tools via a stdlib fetch→cache→**offline**→snapshot module.
- **262,351-vulnerability offline DB** bundled into the vulnerability scanners — real OSV records (CVE/GHSA aliases, CVSS, affected packages) queryable with zero network.
- **Standards exports everywhere** — SARIF (code-scanning), STIX 2.1, OSCAL, Sigma + Suricata, GeoJSON/KML, CSV across the suite.
- **Deeper detection** — C2 campaign correlation, MCP fleet-posture + supply-chain (OWASP Agentic Top-10 2026), maritime track-interaction (CPA/TCPA), and more — each with expanded test suites and candid docs.
- **Passive + authorization-gated active scanning** and polyglot ports rolling out across the scanners.

> Every tool stays single-purpose, self-hostable, **MCP-native**, and defensively-scoped.
<!-- HIGHLIGHTS:END -->

## 🌐 Languages across the suite

**Polyglot by design.** The suite is Python-first but ports outward so a tool exists in the language of *your* deployment target — mainframe to mobile, kernel to contract. **● live in-repo** today, **○ rolling out**.

**● Shipping now**
![Python](https://img.shields.io/badge/Python-3776AB?logo=python&logoColor=white) ![TypeScript](https://img.shields.io/badge/TypeScript-3178C6?logo=typescript&logoColor=white) ![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?logo=javascript&logoColor=black) ![Go](https://img.shields.io/badge/Go-00ADD8?logo=go&logoColor=white) ![Rust](https://img.shields.io/badge/Rust-000000?logo=rust&logoColor=white) ![C%23](https://img.shields.io/badge/C%23-512BD4?logo=dotnet&logoColor=white) ![Ruby](https://img.shields.io/badge/Ruby-CC342D?logo=ruby&logoColor=white) ![Lua](https://img.shields.io/badge/Lua-2C2D72?logo=lua&logoColor=white) ![C](https://img.shields.io/badge/C-A8B9CC?logo=c&logoColor=black) ![COBOL](https://img.shields.io/badge/COBOL-005CA5?logo=gnu&logoColor=white) ![Perl](https://img.shields.io/badge/Perl-39457E?logo=perl&logoColor=white) ![Shell](https://img.shields.io/badge/Shell-4EAA25?logo=gnubash&logoColor=white) ![PowerShell](https://img.shields.io/badge/PowerShell-5391FE?logo=powershell&logoColor=white)

**📱 Mobile — languages & frameworks**
![Swift](https://img.shields.io/badge/Swift-FA7343?logo=swift&logoColor=white) ![Kotlin](https://img.shields.io/badge/Kotlin-7F52FF?logo=kotlin&logoColor=white) ![Java](https://img.shields.io/badge/Java-007396?logo=openjdk&logoColor=white) ![Objective-C](https://img.shields.io/badge/Objective--C-438EFF?logo=apple&logoColor=white) ![Dart](https://img.shields.io/badge/Dart-0175C2?logo=dart&logoColor=white) ![Flutter](https://img.shields.io/badge/Flutter-02569B?logo=flutter&logoColor=white) ![React Native](https://img.shields.io/badge/React_Native-61DAFB?logo=react&logoColor=black)

**⛓️ Smart-contract / Web3** — across EVM · Base · Arbitrum · Blast · Polygon · Solana · Algorand · XRPL · TON · Aptos · Sui · Starknet
![Solidity](https://img.shields.io/badge/Solidity-363636?logo=solidity&logoColor=white) ![Vyper](https://img.shields.io/badge/Vyper-2980B9?logo=python&logoColor=white) ![Move](https://img.shields.io/badge/Move-4A5568?logo=aptos&logoColor=white) ![Cairo](https://img.shields.io/badge/Cairo-EC796B?logo=starknet&logoColor=white) ![Rust](https://img.shields.io/badge/Rust_(Anchor)-000000?logo=rust&logoColor=white) ![Clarity](https://img.shields.io/badge/Clarity-5546FF?logo=stacks&logoColor=white)

<sub>Every port keeps the suite contract: a CLI, structured output (JSON/SARIF), and an MCP server.</sub>

## 👤 Founder & engineering stack

Led by **Christopher Hyatt** — Software & AI Engineer, founder of Cognis Digital, smart-contract auditor (Entersoft, intern→lead), and federal cybersecurity SME. 🛰️ **Hack-A-Sat 7th worldwide / 3,600+ teams** · **Top 1% TryHackMe** · Eagle Scout · CompTIA Security+ · CISSP Prep · AWS DevOps.

**Languages** · Python · TypeScript/JavaScript · Solidity · Rust · Go · Move · Cairo · Vyper · SQL · Bash
**AI / Agents** · self-hosted LLMs · RAG · evals & guardrails · MCP · Claude Agent SDK · LangGraph · CrewAI · AutoGen · LangChain · LlamaIndex · Ollama · vLLM · llama.cpp
**Security & Web3** · smart-contract audit · formal verification · pentesting · Foundry · Slither · Echidna · Mythril · Burp Suite · Nmap · Kali · SIEM/Splunk · NIST 800-53 · MITRE ATT&CK
**Intelligence** · OSINT · SIGINT · GEOINT · HUMINT · ADINT &nbsp;|&nbsp; **PQC** · post-quantum crypto

<sub>Engagements are private and by referral. Development partnerships via <a href="https://devpairer.com">DevPairer</a>.</sub>

## 🤝 Get involved

⭐ **Star the tools you use** — it's the signal that tells us which frontier work to push further · 🛠️ Contribute under the collaboration-pull model (see any repo's `CONTRIBUTING.md`) · 🏢 Commercial use → `licensing@cognis.digital`

<div align="center"><img src="https://capsule-render.vercel.app/api?type=waving&color=0:2b6cb0,100:6b46c1&height=120&section=footer"/></div>

## Interoperability

`cognis-digital` composes across the Cognis suite — JSON in/out and a shared
OpenAI-compatible `/v1` backbone. See **[INTEROP.md](INTEROP.md)** for the
suite map, composition patterns, and reference stacks.

## Integrations

Forward findings to STIX/MISP/Sigma/Splunk/Elastic/Slack/webhooks via
[`cognis-connect`](https://github.com/cognis-digital/cognis-connect). See **[INTEGRATIONS.md](INTEGRATIONS.md)**.

<sub>Featured tables, star counts, and headline numbers on this page are regenerated from the GitHub API daily by <a href="scripts/update_profile.py"><code>scripts/update_profile.py</code></a> — so they never drift from reality.</sub>
