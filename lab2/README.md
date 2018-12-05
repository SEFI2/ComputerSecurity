Welcome to the second lab of this semester.

This lab requires some backgrounds on Python, Flask, and Javascript. Though
the first lab intends to cover some of these, please refer to the following
pages if you are not already familiar with these:

- [Python (Korean)](https://wikidocs.net/book/1)
- [Python (English)](https://docs.python.org/3/tutorial/)
- [JavaScript](https://www.w3schools.com/js/)
- [Flask](http://flask.pocoo.org/docs/1.0/tutorial/)


# 0. Setting

Please be aware that you should work on the following setting when working on
the lab.

- Work only with the chrome browser inside the VM. The Vagrant file is updated
to enable x11 forwarding always, so you should be able to open the browser
through ssh.

- Work inside the VM in which I tested the lab problems.

- Vagrant modified `/etc/hosts` file so that accesses to `example.com`,
    `example.org`, and `example.net` are forwared to `127.0.0.1`, which is
    our VM.

# 1. js

Before working on the real lab problems, please check tho following
links to become familiar with some JavaScript features you may want to
understand to work with the lab.

- [What you can do with JavaScript](https://www.w3schools.com/js/js_intro.asp)
- [alert](https://www.w3schools.com/jsref/met_win_alert.asp)
- [img tab](https://www.w3schools.com/tags/tag_img.asp)
- [onload](https://www.w3schools.com/jsref/event_onload.asp) 

# 2. xss

The second part of lab is about the Cross-Site Scripting (XSS). You will find
the environment at `lab2/xss`.

```
  $ cd xss
```

Under xss, xss.py is the vulnerable web app that we will play with. Let's first
run the app before moving further, with `gunicorn`. `serve-xss.sh` will run the
app with `gunicorn` for you.

```
  $ ./serve-xss.sh
```


Some problems also need the attacker's server
running, with `attacker.py` and `serve-attacker.sh`.

```
  $ ./serve-attacker.sh
```

Then you may want to work on another terminal (you can either use things like
        tmux or run vagrant ssh on other tab of your local terminal emulator.) 


## 1) Stored (15 points)

Looking at the `xss.py`, you will find the function implementing our first page,
`/stored`. Please check the code before moving forward, which has some comments
being the first problem.

### Question

Inject a script invoking `alert()` to let the page pop up an alert screen saying
\"Hello\" through the form for posting. Why is it possible?

### Deliverable

1. `lab2/xss/store.txt` file that has the exact string that you need to pass through the
   form.
2. `lab2/xss/store-desc.txt` file that includes a paragraph describing why
    it was possible, discussing with the store function in the app.


## 2) Reflected (20 points)

As we did previously, you should play with the page `/reflected` this time,
which the reflect function implements.

### Question

This time, craft a URL that the resulting page executes an injected code
invoking `alert()` to let the page pop up an alert screen saying
\"Hello\" through the form for posting. Why is it possible?

### Deliverable

1. `lab2/xss/reflected.txt` file that has the exact URL that you need
   to pass through the form. You should be able to trigger the injection by
   just copy-pasting to the address bar.
2. `lab2/xss/reflected-desc.txt` file that includes a paragraph describing
    why it was possible, discussing with the reflected function in the app.

## 3) dom (20 points)

`dom` function implements the last page, which is simply written in a file
`dom.html`. Our app simply loads and serves it. Please answer this question
by providing the deliverable, being aware that the browser will execute
the injected script unless it renders the page again. However, we can still
trick the browser to run some script that we inject.

### Question

Inject a script invoking `alert()` to let the page pop up an alert screen saying
\"Hello\" through the form for posting. Why is it possible?

### Deliverable

1. `lab2/xss/dom.txt` file that has the exact URL that you need to pass through the
   form. You should be able to trigger the injection by just copy-pasting
   to the address bar.
2. `lab2/xss/dom.txt` file that includes a paragraph describing why it was possible,
    discussing with the page `dom.html`.


## 4) cookie (20 points)

This time let's be a bit more realistic. What bad thing an attacker can do?
As you may have noticed, the `stored` page is setting a cookie when a user
visits the page.

### Question

This time, craft an input that forwards the cookie to the attcker's web app,
when a victim visits the stored page with malicious script injected.

### Deliverable

1. `lab2/xss/cookie.txt` file that has the exact string that you need to pass
    through the posting form.
2. `lab2/xss/cookie-desc.txt` file that includes a paragraph describing
    why it was possible, discussing with the stored function in the app.


# 3. csrf (25 points)

The third part is about Cross-Site Request Forgery, under `./csrf.` `victim.py`
implements our victim that implements simple login and postings, and
`attacker.py` implements the attacker's page. You will also find
`serve-victim.sh` and `serve-attacker.sh` scripts.

### Question

Reading `attacker.py`, you will find that the `/malicious` page from the
attacker's site simply serves contents from `malicious.html`.
Edit `malicious.html` so that if a victim visits
`http://example.org:4444/malicious`, the malicious script will post a string
doomed to the victim's posts page.

### Deliverable

1. `csrf/malicious.html` that works.
2. `csrf/desc.txt` file that explains how malicious.html works.









