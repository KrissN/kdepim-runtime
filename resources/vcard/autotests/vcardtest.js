Resource.setType( "akonadi_vcard_resource" );
Resource.setPathOption( "Path", "vcardtest.vcf" );
Resource.create();

XmlOperations.setXmlFile( "vcardtest.xml" );
XmlOperations.setRootCollections( Resource.identifier() );
XmlOperations.setCollectionKey( "None" ); // we only expect one collection
XmlOperations.ignoreCollectionField( "Name" ); // name is the resource identifier and thus unpredictable
XmlOperations.ignoreCollectionField( "RemoteId" ); // remote id is the absolute path
XmlOperations.assertEqual();

