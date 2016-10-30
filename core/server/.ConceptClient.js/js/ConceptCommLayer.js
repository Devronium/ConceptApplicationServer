'use strict';

function CommLayer(tag) {
    this.Audio = true;
    this.Video = true;
    this.Stream = null;
    this.ReadOnly = false;
    this.Tag = tag ? tag : "";
    this.Peers = [ ];

    window.RTCPeerConnection = window.RTCPeerConnection ? window.RTCPeerConnection : webkitRTCPeerConnection;
    window.RTCPeerConnection = window.RTCPeerConnection ? window.RTCPeerConnection : mozRTCPeerConnection;
    window.RTCIceCandidate = window.RTCIceCandidate ? window.RTCIceCandidate : webkitRTCIceCandidate;
    window.RTCIceCandidate = window.RTCIceCandidate ? window.RTCIceCandidate : mozRTCIceCandidate;

    this.servers = {
        'iceServers':[
            {'urls': 'stun:stun.l.google.com:19302'},
            {'urls': 'turn:192.158.29.39:3478?transport=udp', 'credential': 'JZEOEt2V3Qb0y27GRntt2u2PAYA=', 'username': '28224511:1379330808'},
            {'urls': 'turn:192.158.29.39:3478?transport=tcp', 'credential': 'JZEOEt2V3Qb0y27GRntt2u2PAYA=', 'username': '28224511:1379330808'}
        ]
    };

    this.InitStream = function(stream, local_only) {
        if (!local_only)
            this.Stream = stream;
        this.ReadOnly = local_only;
        this.localPeer = this.NewPeer("local");
        if (!local_only) {
            this.localPeer.addStream(stream);
            var videoTracks = stream.getVideoTracks();
            var audioTracks = stream.getAudioTracks();
            if ((videoTracks) && (videoTracks.length > 0))
                console.log(this.Tag + ': Using video device: ' + videoTracks[0].label);
            if ((audioTracks) && (audioTracks.length > 0))
                console.log(this.Tag + ': Using audio device: ' + audioTracks[0].label);
        }
        if (this.OnLocalStream)
            this.OnLocalStream(stream);
    }

    this.Error = function(e, title) {
        if (e) {
            console.error(this.Tag + ': ' + title + ": ", e.toString());
            if (this.OnError)
                this.OnError(e, title);
        } else
            console.error(this.Tag + ': unknown error');
    }

    this.InitMedia = function(recv_only) {
        if ((this.Stream) || (recv_only)) {
            this.InitStream(this.Stream, recv_only);
            return;
        }
        var width = this.CameraWidth;
        var height = this.CameraHeight;
        var video = this.Video;
        if ((video) && (width) && (width > 0) && (height) && (height > 0)) {
              video = {
                mandatory: {
                  maxWidth: width,
                  maxHeight: height
                }
              };
        }

        navigator.mediaDevices.getUserMedia({
            audio: this.Audio,
            video: video
        }).then(this.InitStream.bind(this)).catch(function(e) {
            alert(this.Tag + ':getUserMedia() error: ' + e.name);
            this.Error(e, "getUserMedia error");
        }.bind(this));
    }

    this.NewPeer = function(name) {
        var pc = new RTCPeerConnection(this.servers);
        this.Peers[this.Peers.length] = pc;
        pc._ref_owner = this;
        pc.onicecandidate = function(e) {
            pc._ref_owner.onIceCandidate(pc, e, name);
        };
        pc.oniceconnectionstatechange = function(e) {
            console.log(pc._ref_owner.Tag + ': ' + name + ' ICE state: ' + pc.iceConnectionState);
            console.log(pc._ref_owner.Tag + ': ICE state change event: ', e);
            if (pc._ref_owner.OnStatusChanged)
                pc._ref_owner.OnStatusChanged(name, pc.iceConnectionState);
        };
        if (pc.ontrack === null) {
            pc.ontrack = function(e) {
                if (pc._ref_owner.OnRemoteStream)
                    pc._ref_owner.OnRemoteStream(e.streams[0]);
            };
        } else {
            pc.onaddstream = function(e) {
                if (pc._ref_owner.OnRemoteStream)
                    pc._ref_owner.OnRemoteStream(e.stream);
            };
        }
        return pc;
    }

    this.onIceCandidate = function(pc, event, name) {
        if (event.candidate)
            this.AddSource(pc, event.candidate);
    }

    this.onAddIceCandidateSuccess = function() {
        console.log(this.Tag + ': addIceCandidate success');
    }

    this.onAddIceCandidateError = function(error) {
        self.Error(error, 'failed to add ICE Candidate');
    } 

    this.AddSource = function(pc, candidate) {
        pc.addIceCandidate(new RTCIceCandidate(candidate)).then(
            function() {
                // add ice candidate success
                if (this.OnICECandidate)
                    this.OnICECandidate(candidate);
            }.bind(this),
            this.onAddIceCandidateError.bind(this)
        );
    }

    this.ICE = function(candidate) {
        if (!this.localPeer) {
            alert("Local peer is not initialized. Call InitMedia first.");
            return;
        }
        this.localPeer.addIceCandidate(
            new RTCIceCandidate(candidate)
        ).then(
            this.onAddIceCandidateSuccess.bind(this),
            this.onAddIceCandidateError.bind(this)
        );
    }

    this.onSetSessionDescriptionError = function(error) {
        self.Error(error, 'Failed to set session description');
    }

    this.onSetLocalSuccess = function(desc) {
        console.log(this.Tag + ': setLocalDescription complete');
    }

    this.onCreateOfferSuccess = function(desc) {
        if (this.OnCreateOffer)
            this.OnCreateOffer(desc);
    }

    this.LocalDescription = function(desc) {
        if (!this.localPeer) {
            alert("Local peer is not initialized. Call InitMedia first.");
            return;
        }
        console.log(this.Tag + ": LOCAL DESCRIPTION");
        this.localPeer.setLocalDescription(desc).then(function() {  this.onSetLocalSuccess(desc); }.bind(this), this.onSetSessionDescriptionError);
        if (this.PendingDescription)
            delete this.PendingDescription;
    }

    this.onSetRemoteSuccess = function(pc) {
        console.log(this.Tag + ': setRemoteDescription complete');
    }

    this.RemoteDescription = function(desc) {
        if (!this.localPeer) {
            alert("Local peer is not initialized. Call InitMedia first.");
            return;
        }
        if (this.PendingDescription) {
            this.LocalDescription(this.PendingDescription);
            delete this.PendingDescription;
        }
        console.log(this.Tag + ": REMOTE DESCRIPTION");
        this.localPeer.setRemoteDescription(desc).then(function() { this.onSetRemoteSuccess(this.localPeer); }.bind(this), this.onSetSessionDescriptionError.bind(this));
    }

    this.onCreateAnswerSuccess = function(desc) {
        console.log(this.Tag + ": Create anwswer ", desc);
        this.LocalDescription(desc);
        if (this.OnCreateAnswer)
            this.OnCreateAnswer(desc);
    }

    this.onCreateSessionDescriptionError = function(error) {
        this.Error(error, 'Failed to create session description');
    }

    this.Answer = function() {
        this.localPeer.createAnswer().then(this.onCreateAnswerSuccess.bind(this), this.onCreateSessionDescriptionError.bind(this));
    }

    this.Offer = function() {
        if (!this.localPeer) {
            alert("Local peer is not initialized. Call InitMedia first.");
            return;
        }
        var pc_remote = this.NewPeer("remote");

        var offerOptions = {offerToReceiveAudio: this.Audio ? 1 : 0, offerToReceiveVideo: this.Video ? 1 : 0}; 
        this.localPeer.createOffer(
            offerOptions
        ).then(
            this.onCreateOfferSuccess.bind(this),
            this.onCreateSessionDescriptionError
        );

        return {"local": this.localPeer, "remote": pc_remote};
    }

    this.Hangup = function() {
        for (var i = 0; i < this.Peers.length; i++) {
            var peer = this.Peers[i];
            if (peer) {
                try {
                    peer.Close();
                } catch (e) {
                    // ignore
                }
            }
        }
        this.Peers = [ ];
        delete this.localPeer;
        if (this.PendingDescription)
            delete this.PendingDescription;
        if (this.Stream) {
            var tracks = this.Stream.getTracks();
            for (var i = 0; i < tracks.length; i++) {
                var track = tracks[i];
                if (track) {
                    try {
                        track.stop();
                    } catch (e) {
                        // ignore
                    }
                }
            }
            this.MediaStream = null;
        }
    }
}
