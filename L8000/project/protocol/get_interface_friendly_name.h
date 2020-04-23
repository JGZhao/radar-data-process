#ifndef GET_INTERFACE_FRIENDLY_NAME_H
#define GET_INTERFACE_FRIENDLY_NAME_H

extern bool parse_as_guid(const char *guid_text, GUID *guid);

/* Get the friendly name for the given GUID */
extern char *get_interface_friendly_name_from_device_guid(GUID *guid);

/*
 * Given an interface name, try to extract the GUID from it and parse it.
 * If that fails, return NULL; if that succeeds, attempt to get the
 * friendly name for the interface in question.  If that fails, return
 * NULL, otherwise return the friendly name, allocated with g_malloc()
 * (so that it must be freed with g_free()).
 */
extern char *get_windows_interface_friendly_name(const char *interface_devicename);

#endif // GET_INTERFACE_FRIENDLY_NAME_H
