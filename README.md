# **bropdox**
A DropBox clone as defined in the final assignment for SISOP 2.\
It is a 2 part assignment and we'll use the UDP sockets API for UNIX in the communication\
between the *Server* and the *Client* threads. 

**Collaborators**:
- Henrique Silva
- Maria Clara Jacintho
- Luis Miguel do Santos Batista
- Vinícius Chagas

---

## **Building the project**
### **Make**
The Makefile has the following interface:
- **all**\
Builds the target `dropboxServer` and `dropboxClient` executables.
- **clean**\
Cleans the generated `.o`'s, binaries and temporary files.
- **redo**\
Does the `all` and then the `clean` rules.
- **test**\
Does the `clean` rule and then nothing because the tests do not exist yet.

---

Extra details can be found in the Moodle course page. I'll later populate this Readme further.