<a name="top"></a>
<div align="center">

<img src="https://capsule-render.vercel.app/api?type=waving&color=0:6b46c1,50:5a3fb0,100:2b6cb0&height=210&section=header&text=COGNIS%20%20DIGITAL&fontSize=66&fontColor=ffffff&fontAlignY=40&stroke=00000022" width="100%"/>

### Making Tomorrow Better Today

<a href="https://cognis.digital"><img src="https://img.shields.io/badge/Web-cognis.digital-6b46c1?style=for-the-badge&logo=google-chrome&logoColor=white"/></a>
<a href="https://github.com/cognis-digital/cognis-neural-suite"><img src="https://img.shields.io/badge/Tools-280%2B-2b6cb0?style=for-the-badge&logo=github&logoColor=white"/></a>
<img src="https://img.shields.io/badge/MCP-native-000000?style=for-the-badge"/>
<img src="https://img.shields.io/badge/License-COCL%201.0-2ea043?style=for-the-badge"/>

<img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=600&size=24&duration=3500&pause=1000&color=6B46C1&center=true&vCenter=true&width=940&height=50&lines=Advanced+Cybersecurity+%E2%80%94+AI+Innovation+%E2%80%94+Blockchain;The+Cognis+Neural+Suite%3A+280%2B+open-source+tools;Single-purpose+%C2%B7+Self-hostable+%C2%B7+MCP-native;Securing+the+agentic+era%2C+one+tool+at+a+time" width="940"/>

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

**Cognis Digital** (Wyoming, USA) builds the **[Cognis Neural Suite](https://github.com/cognis-digital/cognis-neural-suite)** — **280+ single-purpose, self-hostable, MCP-native** tools across security, AI, military/IC, compliance, data, dev, and business. Every tool ships a CLI, JSON/SARIF output, an MCP server, polyglot ports (Py/JS/Go/Rust), a Dev Container, cross-OS + cloud deploy, and CI/CodeQL — and **most ship real, tested code**.

> 🧠 **Every tool is an MCP capability** — point your agents (Claude Desktop, Cursor, Cognis.Studio) at the suite and they can scan, audit, and remediate autonomously.

<div align="center">

<!-- reliable buttons (shields render even when stats services are rate-limited) -->
<a href="https://cognis.digital"><img src="https://img.shields.io/badge/%F0%9F%8C%90_Website-cognis.digital-6b46c1?style=for-the-badge"/></a>
<a href="https://github.com/cognis-digital/cognis-neural-suite"><img src="https://img.shields.io/badge/%F0%9F%97%82%EF%B8%8F_Catalog-280%2B_tools-2b6cb0?style=for-the-badge"/></a>
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

| Flagships | |
|---|---|
| 🤖 [uncensored-fleet](https://github.com/cognis-digital/uncensored-fleet) | local multi-model LLM fleet + harness |
| 💻 [cognis-code](https://github.com/cognis-digital/cognis-code) | local uncensored coding suite for every IDE |
| 🪪 [agentpassport](https://github.com/cognis-digital/agentpassport) | verifiable multi-hop AI-agent identity |
| 🔭 [locateanything](https://github.com/cognis-digital/locateanything) | photo geolocation via local VL + reasoning |
| 🔧 [mcpify](https://github.com/cognis-digital/mcpify) | turn any CLI into an MCP server |
| 🪖 [cognis-neural-suite#military](https://github.com/cognis-digital/cognis-neural-suite) | 12-repo military/IC ecosystem (RMF/STIG/SCIF…) |

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
