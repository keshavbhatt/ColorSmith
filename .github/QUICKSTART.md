# Quick Start: Releasing TorrHunt on Windows

## One-Command Release

```bash
# Update version, commit, tag, and push
VERSION="1.0.0"
git add .
git commit -m "Release v${VERSION}"
git tag -a v${VERSION} -m "Release version ${VERSION}"
git push origin main v${VERSION}
```

## What Happens Next?

1. **GitHub Actions starts automatically** (watch at: Actions tab)
2. **~10-15 minutes later**: Windows executables are ready
3. **Release page updated** with:
   - Installer (recommended)
   - Portable ZIP
   - Checksums for verification
   - Auto-generated release notes

## Testing the Workflow

Want to test without creating a real release?

```bash
# Create a test tag
git tag -a v0.0.1-test -m "Test release"
git push origin v0.0.1-test

# Delete test release and tag later from GitHub UI
```

## Requirements

✅ GitHub repository  
✅ Actions enabled (free for public repos)  
✅ Tag format: `v*.*.*` (e.g., v1.0.0)  
❌ No secrets or tokens needed  

## Workflow Features

- ✅ Builds with Qt 6.4.2
- ✅ Creates Windows installer (Inno Setup)
- ✅ Creates portable ZIP
- ✅ Includes all dependencies
- ✅ SHA256 checksums
- ✅ Auto-generated release notes
- ✅ Artifacts retained for 30 days

## Customization

Edit `.github/workflows/release.yml` to:
- Change Qt version
- Modify installer settings
- Add/remove files
- Change release description
- Add multiple platforms (Linux, macOS)

## Example Workflow

```bash
# 1. Make changes
vim src/main.cpp

# 2. Test locally
cd src && qmake && make

# 3. Commit changes
git add .
git commit -m "Add new feature"

# 4. Release
git tag v1.1.0 -m "Version 1.1.0 - Added new feature"
git push origin main v1.1.0

# 5. Wait ~15 minutes
# 6. Check: https://github.com/YOUR_USERNAME/torrhunt/releases
```

## Troubleshooting

**Build failed?**
- Check Actions log
- Verify Qt version compatibility
- Check .pro file syntax

**Release not created?**
- Ensure tag format is `v*.*.*`
- Check GITHUB_TOKEN permissions
- Verify Actions are enabled

**Missing files in release?**
- Check windeployqt output
- Verify file paths in workflow
- Add missing resources to copy step

## Support

- Workflow issues: Check `.github/workflows/release.yml`
- Build errors: Review Actions logs
- Qt issues: Verify version in workflow matches your development environment

