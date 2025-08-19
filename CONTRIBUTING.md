# Contributing to SkullMasterV2

Thank you for your interest in contributing to SkullMasterV2! This document provides guidelines and instructions for contributing to the project.

## Code of Conduct

By participating in this project, you agree to abide by our [Code of Conduct](CODE_OF_CONDUCT.md). Please read it before contributing.

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check existing issues to avoid duplicates.

When creating a bug report, include:

1. **Clear title and description**
2. **Steps to reproduce**
3. **Expected behavior**
4. **Actual behavior**
5. **Hardware configuration**
   - RP2040 board model
   - Servo types and power supply
   - DMX interface circuit
6. **Software versions**
   - Arduino IDE version
   - Core version (rp2040:rp2040)
   - Library versions
7. **Debug output** (if applicable)
8. **Photos/videos** of the issue (if hardware-related)

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion, include:

1. **Use case** - Why is this enhancement needed?
2. **Proposed solution** - How should it work?
3. **Alternative solutions** - What other approaches did you consider?
4. **Additional context** - Mockups, diagrams, or examples

### Pull Requests

1. **Fork the repository**
2. **Create a feature branch** (`git checkout -b feature/amazing-feature`)
3. **Follow coding standards** (see below)
4. **Test thoroughly** on actual hardware
5. **Update documentation** as needed
6. **Commit changes** with clear messages
7. **Push to your fork** (`git push origin feature/amazing-feature`)
8. **Open a Pull Request**

## Development Process

### Setting Up Development Environment

```bash
# Clone your fork
git clone https://github.com/your-username/SkullMasterV2.git
cd SkullMasterV2

# Add upstream remote
git remote add upstream https://github.com/original-org/SkullMasterV2.git

# Keep your fork updated
git fetch upstream
git checkout main
git merge upstream/main
```

### Coding Standards

Follow the company standards in `/Users/timrosener/Documents/GitHub/STANDARDS/`:

#### Naming Conventions
- **Variables**: `camelCase` for locals, `UPPER_SNAKE_CASE` for constants
- **Functions**: `camelCase` for methods, `PascalCase` for classes
- **Files**: `PascalCase.h` for headers, `lowercase.cpp` for implementation

#### Code Style
```cpp
// Function documentation
/**
 * @brief Calculate servo duty cycle
 * @param servoIndex Index of servo (0-4)
 * @return Duty cycle percentage (0-100)
 */
float getDutyCycle(int servoIndex) {
    // Implementation
}

// Braces on new lines for functions
void function()
{
    // Content
}

// Same line for control structures
if (condition) {
    // Content
}
```

#### Memory Management
- **NO dynamic allocation** in runtime code
- All arrays statically sized
- Use `constexpr` for compile-time constants

#### Safety Requirements
- Always check array bounds
- Validate DMX data before use
- Implement timeout handlers
- Never disable safety features

### Testing Requirements

#### Hardware Testing
1. **Servo Testing**
   - Test all 5 axes independently
   - Verify smooth motion profiles
   - Check limit switches/boundaries
   - Measure power consumption

2. **DMX Testing**
   - Test with professional DMX controller
   - Verify all 512 channels
   - Test signal loss recovery
   - Check isolation and grounding

3. **Integration Testing**
   - Full system test for 8+ hours
   - Temperature monitoring
   - EMI/noise testing
   - Multiple units in parallel

#### Code Testing
```cpp
// Add debug output for testing
#if DEBUG_LEVEL > 0
    Serial.printf("Servo %d: pos=%d, target=%d\n", 
                  i, currentPos, targetPos);
#endif
```

### Documentation Standards

All code changes must include:
1. **Updated inline comments**
2. **Doxygen-style function documentation**
3. **README updates** if features change
4. **API documentation** for new functions
5. **Changelog entry** for notable changes

### Commit Message Format

Follow conventional commits:

```
type(scope): brief description

Longer explanation if needed. Wrap at 72 characters.

Fixes #issue_number
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation only
- `style`: Code style changes
- `refactor`: Code restructuring
- `perf`: Performance improvement
- `test`: Adding tests
- `chore`: Maintenance tasks

Examples:
```
feat(servo): add S-curve motion profile support
fix(dmx): handle corrupted frames gracefully
docs(api): update servo configuration examples
```

## Project Structure

```
SkullMasterV2/
├── SkullMasterV2.ino    # Main sketch - Core 0 and Core 1 logic
├── RS5DMX.h             # DMX and system state
├── RS5DualCore.h        # Inter-core communication
├── RS5Hardware.h        # Hardware configuration
├── ServoDriver.h        # PWM driver implementation
├── ServoEngine.h        # Motion profiling
├── docs/                # Documentation
│   ├── api/            # API reference
│   ├── design/         # Architecture docs
│   └── guides/         # User guides
└── .github/            # GitHub configuration
    ├── ISSUE_TEMPLATE/ # Issue templates
    └── workflows/      # CI/CD workflows
```

## Review Process

### Code Review Checklist

- [ ] Follows coding standards
- [ ] Includes appropriate documentation
- [ ] Has been tested on hardware
- [ ] Doesn't break existing functionality
- [ ] Includes debug output (if applicable)
- [ ] Updates version numbers (if applicable)
- [ ] Memory safe (no dynamic allocation)
- [ ] Thread safe (proper semaphore use)

### Hardware Compatibility

Ensure changes work with:
- Raspberry Pi Pico
- Raspberry Pi Pico W
- Other RP2040 boards
- Various servo types (analog/digital)
- Different DMX controllers

## Release Process

1. **Version Numbering**: Semantic versioning (MAJOR.MINOR.PATCH)
2. **Testing**: Full regression test on reference hardware
3. **Documentation**: Update all docs and changelog
4. **Tag**: Create annotated git tag
5. **Release**: Create GitHub release with notes

## Getting Help

- **Documentation**: Check `docs/` directory first
- **Issues**: Search existing issues
- **Discussions**: Use GitHub Discussions for questions
- **Contact**: Reach out to maintainers

## Recognition

Contributors will be recognized in:
- CHANGELOG.md for their specific contributions
- README.md contributors section (for significant contributions)
- Release notes

## License

By contributing, you agree that your contributions will be licensed under the same CC BY-NC 4.0 license as the project.

## Questions?

Feel free to open an issue with the `question` label or start a discussion.

Thank you for contributing to theatrical magic! 🎭💀