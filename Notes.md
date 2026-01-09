# Git
Right after installation Git does not know who I am
```sh
git config --global user.email "my@epic.email" # GitHub's long e-mail from account settings works.
git config --global user.name "VerMishelb"
```

Without these it won't let you make a commit.
```sh
git config --global core.quotepath false # disables Unicode symbols being displayed in a JSON manner.
```
Mintty terminal settings, "Text", change the font to one that eats Unicode, set the locale to C, encoding to UTF-8. Necessary to fix Cyrillic.
```sh
gir rm --cached # Remove a file from the repository without touching local copy.
```

After creating a repository in GitHub:
```sh
git init
git add .
git commit -m "Initial"
git branch -M master # it's worth noting that "master" is my default branch name, not "main" how it currently is for new accounts.
git remote add origin https://github.com/VerMishelb/CppCmakeGit-learning-minefield.git
git push -u origin master
```

If the repo already existed:
```sh
git remote add origin https://...
git branch -M master
git push -u origin master
```
If we already have files here and there (failed to push some refs):
```sh
git pull --allow-unrelated-histories origin master
git push -u origin master # "master" here is probably the remote's branch, so it may be "main" if github is drunk.
```
DON'T push -f, this will kill anything that's in the remote repo.  
On Linux it'll forget your login credentials every single time. Have to work around that with some side software iirc.

# VSCode extensions
Compared to popular choices these actually work and work correctly.  
* Markdown preview & tools (`Ctrl-Shift-V` for preview) https://marketplace.visualstudio.com/items?itemName=yzhang.markdown-all-in-one  
* CMake syntax highlight https://marketplace.visualstudio.com/items?itemName=KylinIdeTeam.cmake-intellisence  