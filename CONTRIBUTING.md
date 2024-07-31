# Contribution Guide for LiteFM

Thank you for considering contributing to LiteFM! We welcome all kinds of contributions including code, documentation, bug reports, feature requests, and more. This document outlines the process to help you get started.

## Table of Contents
1. [Code of Conduct](#code-of-conduct)
2. [How to Contribute](#how-to-contribute)
   - [Reporting Bugs](#reporting-bugs)
   - [Suggesting Features](#suggesting-features)
   - [Contributing Code](#contributing-code)
   - [Improving Documentation](#improving-documentation)
3. [Development Workflow](#development-workflow)
4. [Style Guides](#style-guides)
   - [Coding Standards](#coding-standards)
   - [Commit Messages](#commit-messages)
5. [Pull Request Process](#pull-request-process)

## Code of Conduct
By participating in this project, you agree to abide by our [Code of Conduct](CODE_OF_CONDUCT.md). Please read it to understand the expectations we have for everyone who participates in the project.

## How to Contribute

### Reporting Bugs
If you find a bug in the project, please open an issue on our [GitHub Issues page](https://github.com/nots1dd/litefm/issues) with the following information:
- A clear and descriptive title.
- Steps to reproduce the bug.
- Expected and actual behavior.
- Screenshots or logs if applicable.
- Any other relevant information.

### Suggesting Features
We welcome new feature suggestions! To propose a feature, please:
- Check existing issues to see if the feature has already been requested.
- Open a new issue and describe the feature in detail.
- Explain why the feature would be beneficial.

### Contributing Code
If you're ready to contribute code, follow these steps:
1. Fork the repository.
2. Create a new branch: `git checkout -b feature/my-feature`.
3. Make your changes and commit them: `git commit -m 'Add some feature'`.
4. Push to the branch: `git push origin feature/my-feature`.
5. Open a pull request.

### Improving Documentation
Documentation improvements are always appreciated! You can:
- Improve existing documentation.
- Add new tutorials or guides.
- Fix typos or clarify language.

## Development Workflow
To set up your development environment:
1. Clone the repository: `git clone https://github.com/nots1dd/litefm.git`
2. Navigate to the project directory: `cd litefm`
3. Follow the setup instructions in the `README.md`.

> [!WARNING]
>
> If you plan on building this manually,
>
> Then ensure that you have all deps installed and working! (check `README.md` for more)

## Style Guides

### Coding Standards
- Follow the existing code style. (TABS AND PRETTY CODE PLS)
- Use meaningful variable and function names.
- Write comments where necessary, especially in complex code sections.
- When creating a new file, ensure that you follow the commenting system in place.

### Commit Messages
- Add a new version file in `CHANGELOG` file with the latest version.md in place
- Ensure that you follow the format of `version.md` and list all important changes
- The commit message MUST follow either `[COMMIT-TYPE] Small msg..` or `[COMMIT-TYPE] CHANGELOG vX.Y`

## Pull Request Process
1. Ensure your changes pass existing tests and do not break the build.
2. Write or update tests as necessary.
3. Submit your pull request with a clear description of your changes.
4. Be prepared to make revisions based on feedback from maintainers.

We appreciate your contributions and look forward to working with you!
