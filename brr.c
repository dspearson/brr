// -*- mode: c; tab-width: 2 -*- //


/*
 * brr.c
 *
 * Copyright (c) 2024, Dominic Pearson <dsp@technoanimal.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#define OPSIZE 10
#define FXSIZE 5
#define INVALID_INPUT fprintf(stderr, "error: invalid input\n");

void usage(const char *self) {
  fprintf(stderr, "Usage: %s <device>\n\n", self);
  fprintf(stderr, "Input: <mode> <power>\n");
  fprintf(stderr, "Modes: {hard, soft, wave, none, quit}\n");
  fprintf(stderr, "Power: [0, 100], for: {hard, soft, wave}\n");
  exit(EXIT_FAILURE);
}

void stop(int dev, int *fxid) {
  if (*fxid != -1) {
    struct input_event evt = { .type = EV_FF, .code = *fxid, .value = 0 };

    if (write(dev, &evt, sizeof(evt)) == -1) {
      perror("error: unable to stop effect");
    }

    *fxid = -1;
  }
}

void apply(int dev, const char *op, int pow, int *fxid) {
  struct ff_effect fx    = { .id = -1, .replay.length = 0xffff, .replay.delay = 0 };
  struct input_event evt = { .type = EV_FF, .value = 1 };

  stop(dev, fxid);

  if (strcmp(op, "sine") == 0) {
    fx.type                              = FF_PERIODIC;
    fx.direction                         = 0x4000;
    fx.u.periodic.envelope.attack_length = 1000;
    fx.u.periodic.envelope.attack_level  = 0x7fff;
    fx.u.periodic.envelope.fade_length   = 1000;
    fx.u.periodic.envelope.fade_level    = 0x7fff;
    fx.u.periodic.magnitude              = 0x7fff * (pow / 100.0);
    fx.u.periodic.period                 = 100;
    fx.u.periodic.waveform               = FF_SINE;
  }

  else if (strcmp(op, "hard") == 0) {
    fx.type                      = FF_RUMBLE;
    fx.u.rumble.strong_magnitude = 0xffff * (pow / 100.0);
    fx.u.rumble.weak_magnitude   = 0;
  }

  else if (strcmp(op, "soft") == 0) {
    fx.type                      = FF_RUMBLE;
    fx.u.rumble.strong_magnitude = 0;
    fx.u.rumble.weak_magnitude   = 0xffff * (pow / 100.0);
  }

  else {
    INVALID_INPUT;
    return;
  }

  if (ioctl(dev, EVIOCSFF, &fx) == -1) {
    perror("error: unable to upload effect");
    exit(EXIT_FAILURE);
  }

  *fxid    = fx.id;
  evt.code = fx.id;

  if (write(dev, &evt, sizeof(evt)) == -1) {
    perror("error: unable to play effect");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    usage(argv[0]);
  }

  const char *path = argv[1];
  int dev          = open(path, O_RDWR);

  if (dev == -1) {
    perror("error: unable to open device");
    exit(EXIT_FAILURE);
  }

  char op[OPSIZE];
  int fxid = -1;

  while (fgets(op, sizeof(op), stdin)) {
    if (strchr(op, '\n') == NULL && strlen(op) >= OPSIZE - 1) {
      INVALID_INPUT;

      int ch;
      while ((ch = getchar()) != '\n' && ch != EOF);

      continue;
    }

    op[strcspn(op, "\n")] = '\0';

    char fx[FXSIZE];
    int pow;

    if (sscanf(op, "%4s %d", fx, &pow) == 2) {
      if (pow < 0 || pow > 100) {
        INVALID_INPUT;
      }

      else {
        apply(dev, fx, pow, &fxid);
      }
    }

    else if (strcmp(op, "none") == 0) {
      stop(dev, &fxid);
    }

    else if (strcmp(op, "quit") == 0) {
      stop(dev, &fxid);
      break;
    }

    else {
      INVALID_INPUT;
    }
  }

  close(dev);
  return 0;
}
