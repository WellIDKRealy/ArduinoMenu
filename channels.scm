(list (channel
        (name 'nonguix)
        (url "file:///etc/system-config/nonguix")
        (branch "trunk")
        (commit
          "20754fe46aa95151c1900a3c479f0cf92addb4b7"))
      (channel
        (name 'guix)
        (url "https://git.savannah.gnu.org/git/guix.git")
        (branch "master")
        (commit
          "ec6d6a51a3ad4c3c0fe90cec1569e29a602d62e2")
        (introduction
          (make-channel-introduction
            "9edb3f66fd807b096b48283debdcddccfea34bad"
            (openpgp-fingerprint
              "BBB0 2DDF 2CEA F6A8 0D1D  E643 A2A0 6DF2 A33A 54FA")))))
