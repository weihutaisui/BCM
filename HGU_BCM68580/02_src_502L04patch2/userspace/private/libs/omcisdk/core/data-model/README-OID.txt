Each object must have an assigned object ID (OID). Objects should get OID's
in the following range:

 2000 - 2099: Broadcom-specific PON objects
 2100 - 2499: ITU GPON objects

The first real object in each cms-dm-*.xml file MUST have an oid=num attribute.
The generate_from_dm.pl script will give all subsequent objects in that file 
the next oid number.

