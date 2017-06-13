/*
dtermn a kinda crummy terminal with a sidebar if you can figure it out
good luck because I haven't written a manpage yet

Copyright (C) 2017  Kyle Dixler

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <gtk-3.0/gdk/gdk.h>
#include <vte/vte.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//Thank you termite for your source code example
gboolean key_press_cb(VteTerminal *vte, GdkEventKey *event) {
    const guint modifiers = event->state & gtk_accelerator_get_default_mod_mask();
    if (modifiers == (GDK_CONTROL_MASK|GDK_SHIFT_MASK)) {
        switch (gdk_keyval_to_lower(event->keyval)) {
            case GDK_KEY_c:
                vte_terminal_copy_clipboard(vte);
                return TRUE;
            case GDK_KEY_v:
                vte_terminal_paste_clipboard(vte);
                return TRUE;
        }
    }
    return FALSE;
}

int main(int argc, char *argv[]){
    GtkWidget *window;
    GtkWidget *terminal;
    GtkWidget *sidebar;
    GtkWidget *pane;
    GPid pid = -1;
    PangoFontDescription *font;
        font = pango_font_description_new();
        pango_font_description_set_family(font, "CMU Typewriter Text");

    /* Initialise GTK, the window and the terminal */
    gtk_init(&argc, &argv);
    // create gtk elements
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    terminal = vte_terminal_new();
    pane = gtk_hpaned_new();


    gtk_window_set_title(GTK_WINDOW(window), "dtermn");

    gtk_paned_add2(GTK_PANED(pane), terminal);


    /* Start a new shell */
    gchar **envp = g_get_environ();
    gchar **command = (gchar *[]){g_strdup(g_environ_getenv(envp, "SHELL")), NULL };
    g_strfreev(envp);
    vte_terminal_spawn_sync(VTE_TERMINAL(terminal),
        VTE_PTY_DEFAULT,
        NULL,       /* working directory  */
        command,    /* command */
        NULL,       /* environment */
        0,          /* spawn flags */
        NULL, NULL, /* child setup */
        &pid,       /* child pid */
        NULL, NULL);


    vte_terminal_set_font_scale (VTE_TERMINAL(terminal), 1.50);
    vte_terminal_set_font (VTE_TERMINAL(terminal), font);

    //Only add sidebar if shell script presented
    if(argc >  1){
        char pid_s[81];
        sprintf(pid_s, "%d", pid, pid);
        //execute first argument in sidebar
        gchar **dirs = (gchar *[]){g_strdup(argv[1]), g_strdup(pid_s), NULL };

        sidebar = vte_terminal_new();
        gtk_paned_add1(GTK_PANED(pane), VTE_TERMINAL(sidebar));
        gtk_paned_set_position(GTK_PANED(pane), 180);
        //gtk_paned_set_position(GTK_PANED(pane), 180);

        vte_terminal_spawn_sync(VTE_TERMINAL(sidebar),
            VTE_PTY_DEFAULT,
            NULL,       /* working directory  */
            dirs,       /* command */
            NULL,       /* environment */
            0,          /* spawn flags */
            NULL, NULL, /* child setup */
            NULL,       /* child pid */
            NULL, NULL);
        vte_terminal_set_cursor_blink_mode (VTE_TERMINAL(sidebar), VTE_CURSOR_BLINK_OFF);
        vte_terminal_set_font_scale (VTE_TERMINAL(sidebar), 1.50);
        vte_terminal_set_font (VTE_TERMINAL(sidebar), font);
    }

    /* Connect some signals */
    g_signal_connect(window, "delete-event", gtk_main_quit, NULL);
    g_signal_connect(VTE_TERMINAL(terminal), "child-exited", gtk_main_quit, NULL);
    g_signal_connect(VTE_TERMINAL(terminal), "key-press-event", G_CALLBACK(key_press_cb), NULL);

    /* Put widgets together and run the main loop */
    gtk_container_add(GTK_CONTAINER(window), pane);
    gtk_widget_show_all(window);
    gtk_widget_grab_focus(terminal);

    gtk_main();
}
