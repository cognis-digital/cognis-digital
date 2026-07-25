#!/usr/bin/env python3
"""Regenerate the live sections of the Cognis Digital profile README.

Everything between the marker pairs below is derived from the GitHub API, so the
featured tables, star counts, headline numbers, and "newly shipped" spotlight can
never drift from reality (which is how the old hand-maintained links rotted after
the rebrand). Runs locally (`gh` authenticated) or in CI (GH_TOKEN in env).

    <!-- cognis:stats:start -->    ... <!-- cognis:stats:end -->
    <!-- cognis:featured:start --> ... <!-- cognis:featured:end -->
    <!-- cognis:frontier:start --> ... <!-- cognis:frontier:end -->

Usage:  python scripts/update_profile.py [--check]
        --check exits non-zero if the README would change (for CI verification).
"""
from __future__ import annotations

import json
import re
import subprocess
import sys
from pathlib import Path

USER = "cognis-digital"
README = Path(__file__).resolve().parent.parent / "README.md"

# repos that are catalogs/meta, not single tools — kept out of the tool tables
META = {"cognis-digital", "cognis-neural-suite", "awesome-cognis", ".github",
        "cognis-code", "cognis"}

# tools renamed to single-word brands — surfaced so the old names still resolve
# (only rendered when the new repo actually exists in the live set)
REBRANDS = {"uefiscan": "bootwarden", "keyhunt": "keyhound", "modpot": "modlure"}

# ordered category rules; a repo lands in the FIRST whose topics/keywords match
CATEGORIES: list[tuple[str, set[str], tuple[str, ...]]] = [
    ("🔴 Threat detection, C2 &amp; DFIR",
     {"dfir", "threat-intelligence", "threat-intel", "forensics", "detection-engineering"},
     ("c2detect", "cobalt", "sliver", "mythic", "havoc", "brute ratel", "beacon",
      "yara", "malware", "packer", "loader", "threat hunt")),
    ("🔧 Firmware · hardware · ICS/OT · RF",
     {"hardware", "firmware", "ics", "iot-security"},
     ("firmware", "uefi", "boot", "sbom", "modbus", "dnp3", "scada", "ble",
      "bluetooth", "can bus", "canbus", "ota", "entropy", "honeypot",
      "secure boot", "bootkit")),
    ("🪖 Defense · national security · geoint",
     {"defense", "defense-tech", "military", "national-security", "dod", "tactical"},
     ("drone", "isr", "sensor fusion", "gps", "gnss", "spoof", "jam", "plume",
      "tak", "counter-uas", "missile", "adsb", "ads-b", "rf ")),
    ("🛰️ OSINT &amp; all-source intelligence",
     {"osint", "intelligence", "reconnaissance", "recon"},
     ("osint", "ais", "maritime", "vessel", "sanction", "blockchain",
      "crypto", "persona", "geoint", "pivot")),
    ("📋 Compliance-as-code &amp; GRC",
     {"compliance", "grc", "rmf", "govtech"},
     ("soc2", "nist", "fedramp", "oscal", "stig", "cmmc", "hipaa", "poam",
      "compliance", "grc", "rmf", "800-53", "iso 27001", "gsa schedule")),
    ("📱 Mobile &amp; application security",
     {"mobile-security"},
     ("apk", "android", "ios", "mobile", "rasp")),
    ("🤖 Agent, MCP &amp; AI security",
     {"agent-security", "ai-security", "mcp"},
     ("mcp", "agent", "prompt", "llm")),
]
PER_CATEGORY = 6
FRONTIER_N = 8


def gh(path: str) -> list[dict]:
    # --jq '.[]' streams one JSON object per line across all pages (robust vs.
    # --paginate's concatenated arrays)
    out = subprocess.run(
        ["gh", "api", "--paginate", path, "--jq", ".[] | tojson"],
        capture_output=True, text=True, check=True,
        encoding="utf-8", errors="replace",
    ).stdout or ""
    return [json.loads(line) for line in out.splitlines() if line.strip()]


def fetch() -> tuple[list[dict], int]:
    repos = [
        r for r in gh(f"users/{USER}/repos?per_page=100&type=owner")
        if not r.get("fork") and not r.get("archived")
    ]
    user = subprocess.run(["gh", "api", f"users/{USER}"],
                          capture_output=True, text=True, check=True,
                          encoding="utf-8", errors="replace").stdout
    followers = json.loads(user).get("followers", 0)
    return repos, followers


def star(n: int) -> str:
    return f" ⭐{n}" if n else ""


def row(r: dict) -> str:
    name, s = r["name"], r.get("stargazers_count", 0)
    desc = (r.get("description") or "").replace("|", "\\|").strip()
    return f"| **[{name}](https://github.com/{USER}/{name})**{star(s)} | {desc} |"


def categorize(repos: list[dict]) -> dict[str, list[dict]]:
    buckets: dict[str, list[dict]] = {c[0]: [] for c in CATEGORIES}
    # match on name + description keywords only — repo topics are over-applied
    # (nearly every security repo carries `threat-intelligence`, `security`, etc.),
    # so they mis-bucket; the curated keyword lists are high-precision.
    for r in repos:
        if r["name"] in META:
            continue
        hay = (r["name"] + " " + (r.get("description") or "")).lower()
        for title, _ctopics, kws in CATEGORIES:
            if any(k in hay for k in kws):
                buckets[title].append(r)
                break
    return buckets


def featured_block(repos: list[dict]) -> str:
    buckets = categorize(repos)
    parts = [
        "## ⚡ Start here — hard-target tools that reach where SaaS doesn't",
        "",
        "**Firmware · ICS/OT · RF · C2 · DFIR · OSINT · compliance-as-code.** "
        "Every tool is single-purpose, **self-hostable**, and emits machine-readable "
        "JSON/SARIF. Counts below are live from the GitHub API.",
        "",
    ]
    for title, _t, _k in CATEGORIES:
        # featured = proven, actually-starred tools; the newest 0-star work is
        # surfaced separately in the Frontier section (so tables aren't padded
        # with alphabetical filler). Tie-break by recency.
        starred = [r for r in buckets[title] if r.get("stargazers_count", 0) > 0]
        picks = sorted(starred,
                       key=lambda r: (r.get("stargazers_count", 0),
                                      r.get("created_at", "")),
                       reverse=True)[:PER_CATEGORY]
        if not picks:
            continue
        parts += [f"### {title}", "", "| Tool | What it does |", "| --- | --- |"]
        parts += [row(r) for r in picks]
        parts.append("")
    return "\n".join(parts).rstrip()


def frontier_block(repos: list[dict], flagship: dict) -> str:
    recent = sorted((r for r in repos if r["name"] not in META),
                    key=lambda r: r.get("created_at", ""), reverse=True)[:FRONTIER_N]
    parts = [
        "## 🌱 Frontier — newly shipped, help them grow",
        "",
        "Our newest work, where the stars haven't caught up to the engineering yet. "
        "If one earns a place in your stack, a ⭐ tells us to push it further.",
        "",
        "| Tool | What it does |",
        "| --- | --- |",
    ]
    parts += [row(r) for r in recent]
    # rebrand note — only for renames whose new repo exists
    live = {r["name"] for r in repos}
    renames = [f"`{o}` → **[{n}](https://github.com/{USER}/{n})**"
               for o, n in REBRANDS.items() if n in live]
    if renames:
        parts += ["",
                  "> **Renamed to single-word brands** (old links now redirect here): "
                  + " · ".join(renames) + " — among others in the rebrand."]
    return "\n".join(parts)


def stats_block(repos: list[dict], followers: int, flagship: dict) -> str:
    n = len(repos)
    stars = sum(r.get("stargazers_count", 0) for r in repos)
    fname, fstars = flagship["name"], flagship.get("stargazers_count", 0)
    b = "https://img.shields.io/badge"
    return "\n".join([
        '<div align="center">', "",
        f'<img src="{b}/Tools-{n}%2B-2b6cb0?style=for-the-badge&logo=github&logoColor=white"/>',
        f'<img src="{b}/Stars-{stars}-6b46c1?style=for-the-badge&logo=github&logoColor=white"/>',
        f'<img src="https://img.shields.io/github/followers/{USER}?style=for-the-badge&label=Followers&color=5a3fb0&logo=github"/>',
        f'<a href="https://github.com/{USER}/{fname}"><img src="{b}/Flagship-{fname}%20%E2%AD%90{fstars}-c0392b?style=for-the-badge"/></a>',
        "",
        f"**{n}** self-hostable, MCP-native tools · **{stars}** stars · flagship "
        f"**[{fname}](https://github.com/{USER}/{fname})** (⭐{fstars})",
        "",
        "</div>",
    ])


def replace(text: str, key: str, block: str) -> str:
    s, e = f"<!-- cognis:{key}:start -->", f"<!-- cognis:{key}:end -->"
    pat = re.compile(re.escape(s) + r".*?" + re.escape(e), re.DOTALL)
    repl = f"{s}\n\n{block}\n\n{e}"
    if not pat.search(text):
        raise SystemExit(f"marker pair {key} not found in README")
    return pat.sub(lambda _m: repl, text)


def main() -> int:
    try:  # Windows consoles default to cp1252 and choke on ⭐ in the summary
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass
    check = "--check" in sys.argv
    repos, followers = fetch()
    flagship = max(repos, key=lambda r: r.get("stargazers_count", 0))
    text = original = README.read_text(encoding="utf-8")
    text = replace(text, "stats", stats_block(repos, followers, flagship))
    text = replace(text, "featured", featured_block(repos))
    text = replace(text, "frontier", frontier_block(repos, flagship))
    if check:
        if text != original:
            print("README is stale — run scripts/update_profile.py")
            return 1
        print("README is up to date")
        return 0
    README.write_text(text, encoding="utf-8")
    print(f"updated: {len(repos)} repos, "
          f"{sum(r.get('stargazers_count', 0) for r in repos)} stars, "
          f"flagship {flagship['name']} ⭐{flagship.get('stargazers_count', 0)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
