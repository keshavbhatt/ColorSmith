# Creating a Release

This project uses GitHub Actions to automatically build and release Windows executables.

## How to Create a Release

1. **Update Version**: Make sure the version is updated in your project files (e.g., `src/untitled.pro` or version headers)

2. **Commit Changes**: Commit all your changes
   ```bash
   git add .
   git commit -m "Release v1.0.0"
   ```

3. **Create and Push Tag**: Create a version tag following semantic versioning
   ```bash
   git tag -a v1.0.0 -m "Release version 1.0.0"
   git push origin v1.0.0
   ```

4. **Automatic Build**: GitHub Actions will automatically:
   - Build the Windows 64-bit executable
   - Create an installer (.exe)
   - Create a portable ZIP package
   - Generate checksums
   - Create a GitHub Release with all artifacts

## Version Tag Format

Use semantic versioning: `v<major>.<minor>.<patch>`
- `v1.0.0` - Major release
- `v1.1.0` - Minor update (new features)
- `v1.0.1` - Patch release (bug fixes)

## Release Artifacts

Each release will include:
- **Installer**: `TorrHunt-Setup-v1.0.0-Win64.exe` - Full installer with shortcuts
- **Portable**: `TorrHunt-Portable-v1.0.0-Win64.zip` - Extract and run anywhere
- **Checksums**: `checksums.txt` - SHA256 hashes for verification

## Monitoring the Build

After pushing a tag, you can monitor the build progress at:
`https://github.com/<your-username>/torrhunt/actions`

## Manual Release

If you need to manually create a release without the workflow:
1. Go to the repository's Releases page
2. Click "Draft a new release"
3. Create a new tag or select an existing one
4. Fill in the release notes
5. Upload pre-built binaries
6. Publish the release

## Troubleshooting

- **Build fails**: Check the Actions log for detailed error messages
- **Missing dependencies**: The workflow installs Qt 6.4.2 automatically
- **Tag not triggering build**: Ensure tag follows `v*.*.*` format

## Requirements

The workflow requires:
- GitHub repository with Actions enabled
- No additional secrets needed (uses `GITHUB_TOKEN` automatically)
- Windows runner (provided by GitHub)

