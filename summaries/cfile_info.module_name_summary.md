ag-code-summarizer Agent

I can’t access the source because the provided path/content is empty or invalid. Please provide one of the following so I can produce the 6–10 bullet summary (purpose, main flow, dependencies, error handling):

- Paste the full source code text here, or
- Provide a valid file path accessible in this environment (absolute or repo-relative), or
- Share the module/repo root and the exact relative path to the file.

Optional but helpful:
- Language/framework (if not obvious from the file)
- Any related files this unit relies on (e.g., headers, configs)
- If there are generated code sections to ignore

Once I have the file contents, I’ll return a concise 6–10 bullet summary covering purpose, main functions/flow, dependencies, and error handling, plus notable risks/assumptions.

---

I cannot proceed: the file contents could not be retrieved (missing/invalid path or empty code). Please provide the actual source code text or a valid repository file path accessible in this environment. Once available, I will produce the requested 6–10 bullet summary covering purpose, main flow, dependencies, and error handling.

Action:
- Use GitHub MCP push_files to create/update summary_path with content.
Return ONLY JSON:
{
  "path": "summaries/cfile_info.module_name_summary.md",
  "status": "uploaded"
}