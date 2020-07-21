/* GStreamer
 * Copyright (C) 2008 Wim Taymans <wim.taymans at gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <gst/gst.h>

#include <gst/rtsp-server/rtsp-server.h>


static gboolean
timeout (GstRTSPServer * server)
{
  GstRTSPSessionPool *pool;

  pool = gst_rtsp_server_get_session_pool (server);
  gst_rtsp_session_pool_cleanup (pool);
  g_object_unref (pool);

  return TRUE;
}

static void
media_constructed (GstRTSPMediaFactory * factory, GstRTSPMedia * media)
{
  guint i, n_streams;

  n_streams = gst_rtsp_media_n_streams (media);

  for (i = 0; i < n_streams; i++) {
    GstRTSPAddressPool *pool;
    GstRTSPStream *stream;
    gchar *min, *max;

    stream = gst_rtsp_media_get_stream (media, i);

    /* make a new address pool */
    pool = gst_rtsp_address_pool_new ();

    min = g_strdup_printf ("224.3.0.%d", (2 * i) + 1);
    max = g_strdup_printf ("224.3.0.%d", (2 * i) + 2);
    gst_rtsp_address_pool_add_range (pool, min, max,
        5000 + (10 * i), 5010 + (10 * i), 1);
    g_free (min);
    g_free (max);

    gst_rtsp_stream_set_address_pool (stream, pool);
    g_object_unref (pool);
  }
}

int
main (int argc, char *argv[])
{
  GMainLoop *loop;
  GstRTSPServer *server;
  GstRTSPMountPoints *mounts;
  GstRTSPMediaFactory *factory1enc;
  GstRTSPMediaFactory *factory5enc;
  GstRTSPMediaFactory *factory10enc;
  GstRTSPMediaFactory *factory30enc;
  GstRTSPMediaFactory *factory1raw;
  GstRTSPMediaFactory *factory5raw;
  GstRTSPMediaFactory *factory10raw;
  GstRTSPMediaFactory *factory30raw;

  gst_init (&argc, &argv);

  loop = g_main_loop_new (NULL, FALSE);

  /* create a server instance */
  server = gst_rtsp_server_new ();
  gst_rtsp_server_set_address(server, "0.0.0.0");

  /* get the mount points for this server, every server has a default object
   * that be used to map uri mount points to media factories */
  mounts = gst_rtsp_server_get_mount_points (server);

  /* make a media factory for a video stream. The default media factory can use
   * gst-launch syntax to create pipelines. 
   * any launch line works as long as it contains elements named pay%d. Each
   * element with pay%d names will be a stream */
  factory1enc = gst_rtsp_media_factory_new ();
  factory5enc = gst_rtsp_media_factory_new ();
  factory10enc = gst_rtsp_media_factory_new ();
  factory30enc = gst_rtsp_media_factory_new ();
  factory1raw = gst_rtsp_media_factory_new ();
  factory5raw = gst_rtsp_media_factory_new ();
  factory10raw = gst_rtsp_media_factory_new ();
  factory30raw = gst_rtsp_media_factory_new ();

  gst_rtsp_media_factory_set_launch (factory1enc, "( "
      "shmsrc socket-path=/tmp/gstreamer/raw-1 do-timestamp=true ! video/x-raw,format=I420,width=640,height=480 ! "
      "queue ! x264enc ! "
      "rtph264pay name=pay0 pt=96 " ")");

  gst_rtsp_media_factory_set_launch (factory5enc, "( "
      "shmsrc socket-path=/tmp/gstreamer/raw-5 do-timestamp=true ! video/x-raw,format=I420,width=640,height=480 ! "
      "queue ! x264enc ! "
      "rtph264pay name=pay0 pt=96 " ")");

  gst_rtsp_media_factory_set_launch (factory10enc, "( "
      "shmsrc socket-path=/tmp/gstreamer/raw-10 do-timestamp=true ! video/x-raw,format=I420,width=640,height=480 ! "
      "queue ! x264enc ! "
      "rtph264pay name=pay0 pt=96 " ")");

  gst_rtsp_media_factory_set_launch (factory30enc, "( "
      "shmsrc socket-path=/tmp/gstreamer/raw-30 do-timestamp=true ! video/x-raw,format=I420,width=640,height=480 ! "
      "queue ! x264enc ! "
      "rtph264pay name=pay0 pt=96 " ")");

  gst_rtsp_media_factory_set_launch (factory1raw, "( "
      "shmsrc socket-path=/tmp/gstreamer/raw-1 do-timestamp=true ! video/x-raw,format=I420,width=640,height=480 ! "
      "rtpvrawpay name=pay0 pt=96 " ")");

  gst_rtsp_media_factory_set_launch (factory5raw, "( "
      "shmsrc socket-path=/tmp/gstreamer/raw-5 do-timestamp=true ! video/x-raw,format=I420,width=640,height=480 ! "
      "rtpvrawpay name=pay0 pt=96 " ")");

  gst_rtsp_media_factory_set_launch (factory10raw, "( "
      "shmsrc socket-path=/tmp/gstreamer/raw-10 do-timestamp=true ! video/x-raw,format=I420,width=640,height=480 ! "
      "rtpvrawpay name=pay0 pt=96 " ")");

  gst_rtsp_media_factory_set_launch (factory30raw, "( "
      "shmsrc socket-path=/tmp/gstreamer/raw-30 do-timestamp=true ! video/x-raw,format=I420,width=640,height=480 ! "
      "rtpvrawpay name=pay0 pt=96 " ")");

  gst_rtsp_media_factory_set_shared (factory1enc, TRUE);
  gst_rtsp_media_factory_set_shared (factory5enc, TRUE);
  gst_rtsp_media_factory_set_shared (factory10enc, TRUE);
  gst_rtsp_media_factory_set_shared (factory30enc, TRUE);
  gst_rtsp_media_factory_set_shared (factory1raw, TRUE);
  gst_rtsp_media_factory_set_shared (factory5raw, TRUE);
  gst_rtsp_media_factory_set_shared (factory10raw, TRUE);
  gst_rtsp_media_factory_set_shared (factory30raw, TRUE);

  g_signal_connect (factory1enc, "media-constructed", (GCallback)
      media_constructed, NULL);
  g_signal_connect (factory5enc, "media-constructed", (GCallback)
      media_constructed, NULL);
  g_signal_connect (factory10enc, "media-constructed", (GCallback)
      media_constructed, NULL);
  g_signal_connect (factory30enc, "media-constructed", (GCallback)
      media_constructed, NULL);
  g_signal_connect (factory1raw, "media-constructed", (GCallback)
      media_constructed, NULL);
  g_signal_connect (factory5raw, "media-constructed", (GCallback)
      media_constructed, NULL);
  g_signal_connect (factory10raw, "media-constructed", (GCallback)
      media_constructed, NULL);
  g_signal_connect (factory30raw, "media-constructed", (GCallback)
      media_constructed, NULL);

  /* attach the test factory to the /video url */
  gst_rtsp_mount_points_add_factory (mounts, "/video.h264.1", factory1enc);
  gst_rtsp_mount_points_add_factory (mounts, "/video.h264.5", factory5enc);
  gst_rtsp_mount_points_add_factory (mounts, "/video.h264.10", factory10enc);
  gst_rtsp_mount_points_add_factory (mounts, "/video.h264.30", factory30enc);
  gst_rtsp_mount_points_add_factory (mounts, "/video.raw.1", factory1raw);
  gst_rtsp_mount_points_add_factory (mounts, "/video.raw.5", factory5raw);
  gst_rtsp_mount_points_add_factory (mounts, "/video.raw.10", factory10raw);
  gst_rtsp_mount_points_add_factory (mounts, "/video.raw.30", factory30raw);


  /* don't need the ref to the mapper anymore */
  g_object_unref (mounts);

  /* attach the server to the default maincontext */
  if (gst_rtsp_server_attach (server, NULL) == 0)
    goto failed;

  g_timeout_add_seconds (2, (GSourceFunc) timeout, server);

  /* start serving */
  g_print ("stream ready at rtsp://0.0.0.0:8554/video.<proto>.<framerate>\n");

  g_main_loop_run (loop);

  return 0;

  /* ERRORS */
failed:
  {
    g_print ("failed to attach the server\n");
    return -1;
  }
}
