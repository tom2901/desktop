/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "../shared/strdup.h"




GtkWidget *win, *label, *image, *box, *path, *clear, *choose, *button_box, *apply;




gboolean clear_wallpaper(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  gtk_label_set_text(GTK_LABEL(path), "file://");
  gtk_image_set_from_icon_name(GTK_IMAGE(image), "gtk-missing-image", GTK_ICON_SIZE_DIALOG);
  return FALSE;
}

gboolean open_wallpaper(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  GtkWidget *dialog = gtk_file_chooser_dialog_new("Open Wallpaper", GTK_WINDOW(win), GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
  GtkFileFilter *pics = gtk_file_filter_new();
  gtk_file_filter_add_pixbuf_formats(pics);
  gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), pics);

  gint res = gtk_dialog_run(GTK_DIALOG(dialog));
  if(res == GTK_RESPONSE_ACCEPT)
    {
      char *f = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
      char *p = malloc(strlen(f) + 10);
      snprintf(p, strlen(f) + 10, "file://%s", f);
      gtk_label_set_text(GTK_LABEL(path), p);

      char *pic_path = strdup(p);
      pic_path = strtok(pic_path, "//");
      pic_path = strtok(NULL, "\0");
      gtk_image_set_from_pixbuf(GTK_IMAGE(image), gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file(pic_path, NULL), 450, 250, GDK_INTERP_NEAREST));

      free(p);
    }

  gtk_widget_destroy (dialog);
  return FALSE;
}

gboolean write_wallpaper_settings(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  GSettings *wp = g_settings_new("org.gnome.desktop.background");
  g_settings_set_value(wp, "picture-uri", g_variant_new_string(gtk_label_get_text(GTK_LABEL(path))));
  return FALSE;
}

gboolean destroy(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  gtk_main_quit();
  return FALSE;
}

gboolean wallpaper_settings(void)
{
    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_resize(GTK_WINDOW(win), 800, 500);
    gtk_window_set_title(GTK_WINDOW(win), "Settings - Wallpaper");
    gtk_container_set_border_width(GTK_CONTAINER(win), 10);
    g_signal_connect(G_OBJECT(win), "delete-event", G_CALLBACK(destroy), NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    label = gtk_label_new("On This page you can setup your Wallpaper:");

    //Get Current Wallpaper
    GSettings *gnome_conf;
    gnome_conf = g_settings_new("org.gnome.desktop.background");
    const char *ptr = g_variant_get_string(g_settings_get_value(gnome_conf, "picture-uri"), NULL);
    g_debug("Loading Wallpaper %s", ptr);

    //Setup GtkImage
    char *pic_path = strdup(ptr);
    pic_path = strtok(pic_path, "//");
    pic_path = strtok(NULL, "\0");
    image = gtk_image_new_from_file(pic_path);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), gdk_pixbuf_scale_simple(gtk_image_get_pixbuf(GTK_IMAGE(image)), 450, 250, GDK_INTERP_NEAREST));

    button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_set_homogeneous(GTK_BOX(button_box), TRUE);

    clear  = gtk_button_new_with_label("Clear");
    gtk_container_add(GTK_CONTAINER(button_box), clear);
    g_signal_connect(G_OBJECT(clear), "button_press_event", G_CALLBACK(clear_wallpaper), NULL);

    choose = gtk_button_new_with_label("Open...");
    gtk_container_add(GTK_CONTAINER(button_box), choose);
    g_signal_connect(G_OBJECT(choose), "button_press_event", G_CALLBACK(open_wallpaper), NULL);

    path = gtk_label_new(ptr);

    apply = gtk_button_new_with_label("Apply");
    g_signal_connect(G_OBJECT(apply), "button_press_event", G_CALLBACK(write_wallpaper_settings), NULL);

    gtk_container_add(GTK_CONTAINER(box), label);
    gtk_container_add(GTK_CONTAINER(box), image);
    gtk_container_add(GTK_CONTAINER(box), path);
    gtk_container_add(GTK_CONTAINER(box), button_box);
    gtk_container_add(GTK_CONTAINER(box), apply);

    gtk_container_add(GTK_CONTAINER(win), box);
    gtk_widget_show_all(win);
    return FALSE;
}


int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  wallpaper_settings();

  gtk_main();
}
