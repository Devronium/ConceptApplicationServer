USER = @USER@
GROUP = @GROUP@

EXTRA_DIST = Certificates Include Public Samples

prefix=@prefix@

install-data-hook:
	mkdir -p @prefix@/etc/concept/Include
	mkdir -p @prefix@/etc/concept/Certificates
	mkdir -p @prefix@/etc/concept/Samples
	mkdir -p @prefix@/etc/concept/Public
	mkdir -p @prefix@/etc/concept/Services
	cp -R Include @prefix@/etc/concept/
	cp -R Certificates @prefix@/etc/concept/
	cp -R Public @prefix@/etc/concept/
	cp -R Samples @prefix@/etc/concept/
	cp -R Services @prefix@/etc/concept/
	chmod -R 755 @prefix@/etc/concept
	cp ConceptServer.con @prefix@/sbin/
	chmod 755 @prefix@/sbin/ConceptServer.con
	cp ConceptServerSetup.con @prefix@/sbin/
	chmod 755 @prefix@/sbin/ConceptServerSetup.con
	cp ConceptSSLProxyServer.con @prefix@/sbin/
	chmod 755 @prefix@/sbin/ConceptSSLProxyServer.con
	cp ConceptSSLProxyServer.con.manifest @prefix@/sbin/

uninstall-hook:
	rm -rf @prefix@/etc/concept/Samples
	rm -rf @prefix@/etc/concept/Include
	rm -rf @prefix@/etc/concept/Certificates
	rm @prefix@/sbin/ConceptServer.con
	rm @prefix@/sbin/ConceptServerSetup.con
	rm @prefix@/sbin/ConceptSSLProxyServer.con
	rm @prefix@/sbin/ConceptSSLProxyServer.con.manifest

#Includedir=@prefix@/etc/concept/Include
#Certificatesdir=@prefix@/etc/concept/Certificates
#Publicdir=@prefix@/etc/concept/Public
#Samplesdir=@prefix@/etc/concept/Samples
#Include_DATA=Include
#Certificates_DATA=Certificates
#Public_DATA=Public
#Samples_DATA=Samples

#prefix = @prefix@
#pkgdata_DATA = Certificates Include Public Samples
#EXTRA_DIST = $(pkgdata_DATA)
#MANTAINERCLEANFILES = $(pkgdata_DATA)
#PKGDIR = @prefix@/etc/concept
#pkgdatadir = @prefix@/etc/concept

#pkgdatadir = @pkgdatadir@
#$(prefix)/share/Concept

#install:
#	mkdir -p $DEST_DIR
#	chown $(USER) $(DEST_DIR)
#	chgrp $(GROUP) $(DEST_DIR)
#	chmod 755 $(DEST_DIR)
#	cp loading.gif $(DEST_DIR)/loading.gif
#	cp splash.jpg $(DEST_DIR)/splash.jpg
