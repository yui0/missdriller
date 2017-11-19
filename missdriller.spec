Name: missdriller
Summary: Mr. Driller (C)NAMCO clone
Version: 0.1
Release: b2
URL: http://www.geocities.co.jp/Berkeley/2093/driller.html
# Ref. http://www.embeddev.se/agroot/start.html
Source0: %name-%version.tar.bz2
License: LGPL
Group: Amusements/Games
Buildroot: %{_tmppath}/%{name}-%{version}
Buildrequires: SDL-devel, SDL_mixer-devel
Requires: SDL_mixer >= 1.2.2


%description
Miss Driller, a high quality addictive game using SDL.


%prep
%setup -q

%build
#make OPTIMIZE="$RPM_OPT_FLAGS" PREFIX=%buildroot%{_prefix}
make -C src


%install
rm -rf $RPM_BUILD_ROOT

install -d $RPM_BUILD_ROOT/opt/game/%name/data/{bmp,mod,susumi,wav}
strip src/drill
install -m 755 src/drill $RPM_BUILD_ROOT/opt/game/%name
cp -a data/drill.ini $RPM_BUILD_ROOT/opt/game/%name/data/
cp -a data/drill.scr $RPM_BUILD_ROOT/opt/game/%name/data/
cp -a data/drilltime.scr $RPM_BUILD_ROOT/opt/game/%name/data/
#cp -a engrave16.bmp $RPM_BUILD_ROOT/opt/game/%name
cp -a data/engrave8.bmp $RPM_BUILD_ROOT/opt/game/%name/data/
cp -a data/blockbg.txt $RPM_BUILD_ROOT/opt/game/%name/data/
cp -a data/charabg.txt $RPM_BUILD_ROOT/opt/game/%name/data/
cp -a data/otherbg.txt $RPM_BUILD_ROOT/opt/game/%name/data/
cp -a data/wav.txt $RPM_BUILD_ROOT/opt/game/%name/data/
#cp -a COPYING $RPM_BUILD_ROOT/opt/game/%name
#cp -a readme.txt $RPM_BUILD_ROOT/opt/game/%name
#cp -a arrows/* $RPM_BUILD_ROOT/opt/game/%name/arrows
#cp -a player/* $RPM_BUILD_ROOT/opt/game/%name/player
cp -a data/susumi/* $RPM_BUILD_ROOT/opt/game/%name/data/susumi
cp -a data/bmp/* $RPM_BUILD_ROOT/opt/game/%name/data/bmp
cp -a data/wav/* $RPM_BUILD_ROOT/opt/game/%name/data/wav
cp -a data/mod/* $RPM_BUILD_ROOT/opt/game/%name/data/mod

#install -d $RPM_BUILD_ROOT/etc/X11/applnk/Games
#install -m 644 kde/driller.desktop $RPM_BUILD_ROOT/etc/X11/applnk/Games
install -m 644 kde/%name.png $RPM_BUILD_ROOT/opt/game/%name

mkdir -p $RPM_BUILD_ROOT%{_datadir}/applications
desktop-file-install --vendor=%name              \
  --dir $RPM_BUILD_ROOT%{_datadir}/applications  \
  --add-category X-Red-Hat-Base                  \
  --add-category Application                     \
  --add-category Game                            \
  kde/%{name}.desktop

# install icons and GNOME desktop entry
#cp -a icons $RPM_BUILD_ROOT/usr/share/%name
#cat >%name.desktop <<EOF
#[Desktop Entry]
#Name=Frozen Bubble
#TryExec=frozen-bubble
#Exec=frozen-bubble
#Icon=%{_prefix}/share/frozen-bubble/icons/frozen-bubble-icon-32x32.png
#Terminal=0
#Type=Application
#EOF
#mkdir -p $RPM_BUILD_ROOT%{_datadir}/applications
#desktop-file-install --vendor=%name              \
#  --dir $RPM_BUILD_ROOT%{_datadir}/applications  \
#  --add-category X-Red-Hat-Extra                 \
#  --add-category Application                     \
#  --add-category Game                            \
#  %{name}.desktop


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root)
/opt/game/%name/*
##/etc/X11/applnk/Games/*
%{_datadir}/applications
#%doc AUTHORS CHANGES COPYING README INSTALL
#%{_prefix}/bin/*
#%{_prefix}/lib/frozen-bubble
#%{_prefix}/share/frozen-bubble
#%{_datadir}/applications
# %{_prefix}/share/man/man3/*
#%{_prefix}/share/man/man6/*

%changelog
* Sun Feb 6 2011 Yuichiro Nakada <berry@rberry.co.cc> - 0.1-b1
- Update
* Fri Oct 3 2003 Yuichiro Nakada <berry@po.yui.mine.nu>
- Create for Berry Linux
