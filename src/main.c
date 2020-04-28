#include <gtk/gtk.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


typedef struct {
  GtkWidget* drive;
  GtkWidget* file;
  GtkWidget* write;
} multi_widgets;

char cmd[255];

void update_drives(GtkComboBoxText* btn_select_drive);

void window_build(int argc, char *argv[]) {
  GtkBuilder*    builder;
  GtkWidget*     window;

  multi_widgets* widgets = g_slice_new(multi_widgets);

  gtk_init(&argc, &argv);

  builder = gtk_builder_new_from_file ("glade/window_main.glade");
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));

  widgets->file      = GTK_WIDGET(gtk_builder_get_object(builder, "btn_file_open"));
  widgets->drive     = GTK_WIDGET(gtk_builder_get_object(builder, "btn_select_drive"));
  widgets->write     = GTK_WIDGET(gtk_builder_get_object(builder, "btn_write"));

  update_drives(GTK_COMBO_BOX_TEXT(widgets->drive));

  gtk_builder_connect_signals(builder, widgets);
  g_object_unref(builder);

  gtk_widget_show_all(window);
  gtk_main();
  g_slice_free(multi_widgets, widgets);
}


void unblock_write(multi_widgets* widgets) {
  gchar* file_name  = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widgets->file));
  gchar* drive_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets->drive));

  if ((file_name != NULL) && (drive_name != NULL)) {
    gtk_widget_set_sensitive(widgets->write, TRUE);
  }

  g_free(file_name);
  g_free(drive_name);
}

void block_all(multi_widgets* widgets) {
  gtk_widget_set_sensitive(widgets->file,  FALSE);
  gtk_widget_set_sensitive(widgets->drive, FALSE);
  gtk_widget_set_sensitive(widgets->write, FALSE);
}

void unblock_all(multi_widgets* widgets) {
  gtk_widget_set_sensitive(widgets->file,  TRUE);
  gtk_widget_set_sensitive(widgets->drive, TRUE);
  gtk_widget_set_sensitive(widgets->write, TRUE);
}

void update_drives(GtkComboBoxText* btn_select_drive) {
  char *cmd = "lsblk -nd --output NAME,SIZE,MODEL,TYPE | grep disk | awk '{ print $3 }'";
  char line[128];
  FILE *fp;

  if ((fp = popen(cmd, "r")) == NULL) {
    printf("Error opening pipe!\n");
    exit(-1);
  }

  gtk_combo_box_text_remove_all(btn_select_drive);
  while (fgets(line, sizeof(line), fp)) {
    gtk_combo_box_text_append_text(btn_select_drive, line);
  }
}

void get_drive_letter(char* drive_letter, char* drive_name) {
  char cmd[255];
  FILE *fp;

  sprintf(cmd, "lsblk -nd --output NAME,SIZE,MODEL,TYPE | grep disk | grep %s | awk '{ print $1 }'", drive_name);

  if ((fp = popen(cmd, "r")) == NULL) {
    printf("Error opening pipe!\n");
    exit(-1);
  }

  fgets(drive_letter, sizeof(drive_letter), fp);
}

void cmd_run(char* cmd) {
  sleep(2);
  printf("cmd: %s\n", cmd);
  //int result = system(cmd);
  //printf("return: %d\n", result);
  sleep(8);
  printf("complete\n");
}

void write_iso(multi_widgets* widgets) {
  pthread_t cmd_thread;
  // char cmd[255];
  char drive_letter[20];
  char* drive;

  gchar* file_name  = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widgets->file));

  gchar* drive_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets->drive));
  drive_name = strtok(drive_name, "\n");

  get_drive_letter(drive_letter, drive_name);
  drive = strtok(drive_letter, "\n");

  sprintf(cmd, "dd if=%s of=/dev/%s bs=1M", file_name, drive);
  cmd_run (cmd);
  // pthread_create(&cmd_thread, NULL, (void * (*)(void*)) cmd_run, cmd);
  // pthread_join(cmd_thread, NULL);

  g_free(file_name);
  g_free(drive_name);
}

void on_focus_btn_select_drive(GtkComboBoxText* btn_select_drive, multi_widgets* widgets) {
  update_drives(btn_select_drive);
}

void on_show_window_main(GtkWidget* window_main, multi_widgets* widgets){
  update_drives(GTK_COMBO_BOX_TEXT(widgets->drive));
}

void on_change_btn_select_drive(GtkWidget* btn_select_drive, multi_widgets* widgets) {
  unblock_write(widgets);
}

void on_change_btn_file_open(GtkWidget* btn_file_open, multi_widgets* widgets) {
  unblock_write(widgets);
}

void on_clicked_btn_write(GtkWidget* write, multi_widgets* widgets) {
  block_all(widgets);
  write_iso(widgets);
  unblock_all(widgets);
}

void on_window_main_destroy() {
  gtk_main_quit();
}

int main(int argc, char *argv[]) {
  window_build(argc, argv);
  return 0;
}

