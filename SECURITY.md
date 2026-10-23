# dbus-python security

dbus-python is only minimally maintained,
and is not recommended for use in contexts where it is security-sensitive.

dbus-python's design is not well-suited to being used in security-sensitive
contexts.
Its interface involves guessing D-Bus types dynamically from
user-specified values,
and this cannot be changed without breaking backward compatibility.
In security-sensitive contexts such as D-Bus system services,
please consider using a better-designed library.

Users of this library are responsible for ensuring that all values that
will be appended to D-Bus messages are well-formed.
In particular,
if a bytestring (the `bytes` type) is used as a D-Bus string (`s`),
it must be valid UTF-8 without overlong sequences,
and all strings (both `str` and `bytes`) must be valid Unicode
with no codepoints higher than U+10FFFF,
and additionally must not contain U+0000.

## Reporting security vulnerabilities

If you find a security vulnerability that is not known to the public,
please report it privately to <dbus-security@lists.freedesktop.org>,
or by reporting a Gitlab issue at
<https://gitlab.freedesktop.org/dbus/dbus-python/-/work_items/new>
and marking it as "confidential".

If you are unsure whether a bug is a security vulnerability or not,
please report it as confidential.

If you are proposing a fix for a security vulnerability,
please create a "confidential" merge request from a **private** fork of the
repository.
See <https://docs.gitlab.com/ee/user/project/merge_requests/confidential.html>
for details.
Do not push security fixes for undisclosed vulnerabilities to a public fork
or create a public merge request for them.

If you are unsure whether a fork or a merge request will become public,
please do not push your proposed fix at all:
it is safer to generate patches with `git format-patch` and attach them
to the confidential issue report.

## CVE IDs

If you allocate a CVE ID or similar unique identifier for a vulnerability,
please leave a comment with the CVE ID on the relevant issue report,
or create a new issue report if necessary
(marked as confidential if the vulnerability is not yet known to the public).
The dbus maintainers cannot provide support for CVE IDs that are allocated
without their knowledge.
