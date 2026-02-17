# Architecture Diagrams

This directory contains Mermaid diagrams that visualize render engine architecture.

## Diagrams

| Diagram | Description |
|---------|-------------|
| [`high-level.mmd`](high-level.mmd) | High-level architecture showing core libraries, dependencies, and applications |
| [`component-diagram.mmd`](component-diagram.mmd) | Detailed component breakdown of each core module |
| [`request-flow.mmd`](request-flow.mmd) | Sequence diagram showing typical render request flow |
| [`deployment.mmd`](deployment.mmd) | Deployment and build process diagram |

## Generating SVG Images

To render Mermaid diagrams as SVG files (for better GitHub display), use the Mermaid CLI:

```bash
# Install the CLI (one-time)
npm install -g @mermaid-js/mermaid-cli

# Generate SVG files
mmdc -i high-level.mmd -o assets/high-level.svg
mmdc -i component-diagram.mmd -o assets/component-diagram.svg
mmdc -i request-flow.mmd -o assets/request-flow.svg
mmdc -i deployment.mmd -o assets/deployment.svg
```

**Note:** The diagrams can also be rendered directly by GitHub (which has built-in Mermaid support). The SVG versions are provided for:
- Better compatibility with README viewers that don't support Mermaid
- Faster loading
- Consistent rendering across platforms

## Updating Diagrams

1. Edit the `.mmd` source files
2. Regenerate SVG files using the commands above
3. Update references in the main README.md if needed

## Using Diagrams in Markdown

GitHub supports both inline Mermaid and image references:

```markdown
<!-- Inline Mermaid (auto-rendered by GitHub) -->
```mermaid
graph TD
    A --> B
```

<!-- Image reference (pre-rendered SVG) -->
![High-level architecture](assets/high-level.svg)
```

This README uses both approaches for maximum compatibility.
