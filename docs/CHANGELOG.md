# Changelog

All notable changes to the SkullMasterV2 project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### To Do
- Add telemetry output for remote monitoring
- Implement servo position save/recall
- Add choreography sequence playback

## [3.1.0-alpha] - 2024-12-27
### Added
- Documentation following company standards
- Comprehensive API documentation
- Hardware setup guides
- Architecture documentation

### Changed
- Migrated to arduino-pico core for FreeRTOS support
- Updated compilation process for new toolchain

### Fixed
- PWM timing issues on certain servo models
- DMX packet validation improvements

## [3.0.0] - 2024
### Added
- Multi-island configuration support for pirate show
- Dynamic eye DMX addressing based on location
- Flicker effect profiles for different scenes
- Per-servo status LED indicators
- Servo sleep mode for power saving

### Changed
- Refactored to dual-core architecture
- Separated DMX processing from servo control
- Improved motion profiling with acceleration limits
- Enhanced debug output system

### Removed
- Legacy single-core implementation
- Serial control mode (DMX only now)

## [2.0.0] - 2023
### Added
- Pirates of the Caribbean themed eye effects
- Location-specific DMX configurations
- Support for 5 servo axes
- NeoPixel status display system

### Changed
- Migrated from ESP32 to RP2040 platform
- Implemented hardware PWM using PIO
- New motion engine with smooth acceleration

## [1.0.0] - 2022
### Added
- Initial release for Rose&Swan productions
- Basic DMX control for animatronic skull
- Jaw, yaw, and pitch control
- Simple LED eye control
- Debug output via serial

## Version Numbering

- **Major (X.0.0)**: Breaking changes to DMX protocol or hardware
- **Minor (0.X.0)**: New features, backwards compatible
- **Patch (0.0.X)**: Bug fixes and minor improvements
- **Alpha/Beta**: Pre-release testing versions