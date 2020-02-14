# setup-doxygen

This action sets doxygen environment for use in actions by:

- downloading and caching a version of doxygen and add to PATH

# Usage

See [action.yml](action.yml)

Basic:
```yaml
steps:
- uses: actions/checkout@v1
- uses: .github/actions/setup-doxygen
  with:
    doxygen-version: '1.8.17'
- run: doxygen
```
