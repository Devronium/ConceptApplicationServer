'use strict';

function CommLayer(tag) {
    this.Audio = true;
    this.Video = true;
    this.Stream = null;
    this.ReadOnly = false;
    this.Tag = tag ? tag : "";
    this.Peers = [ ];
    this.localPeers = { };
    this.PendingChannelDescription = { };
    this.StreamCount = 0;

    window.RTCPeerConnection = window.RTCPeerConnection ? window.RTCPeerConnection : webkitRTCPeerConnection;
    window.RTCPeerConnection = window.RTCPeerConnection ? window.RTCPeerConnection : mozRTCPeerConnection;
    window.RTCIceCandidate = window.RTCIceCandidate ? window.RTCIceCandidate : webkitRTCIceCandidate;
    window.RTCIceCandidate = window.RTCIceCandidate ? window.RTCIceCandidate : mozRTCIceCandidate;

    this.servers = {
        'iceServers':[
            {'urls': 'stun:stun.l.google.com:19302'}
        ]
    };

    this.GetPeer = function(name) {
        if ((name) && (name.length > 0))
            return this.localPeers[name];
        return this.localPeer;
    }

    this.InitStream = function(stream, local_only, stream_name) {
        if (!local_only)
            this.Stream = stream;
        this.ReadOnly = local_only;
        var peer = this.NewPeer("local", stream_name);
        if ((stream_name) && (stream_name.length > 0)) {
            this.localPeers[stream_name] = peer;
        } else {
            this.localPeer = peer;
            stream_name = "";
        }
        if (!local_only) {
            peer.addStream(stream);
            var videoTracks = stream.getVideoTracks();
            var audioTracks = stream.getAudioTracks();
            if ((videoTracks) && (videoTracks.length > 0))
                console.log(this.Tag + ': Using video device: ' + videoTracks[0].label);
            if ((audioTracks) && (audioTracks.length > 0))
                console.log(this.Tag + ': Using audio device: ' + audioTracks[0].label);
        }

        if (this.OnLocalStream)
            this.OnLocalStream(stream, stream_name);
    }

    this.Error = function(e, title, stream_name) {
        if (e) {
            console.error(this.Tag + ': ' + title + ": ", e.toString());
            if (this.OnError)
                this.OnError(e, title, stream_name);
        } else
            console.error(this.Tag + ': unknown error');
    }

    this.InitMedia = function(recv_only, stream_name) {
        if ((this.Stream) || (recv_only)) {
            this.InitStream(this.Stream, recv_only, stream_name);
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
              if (this.Video == 2) {
                  video.mandatory.chromeMediaSource = 'screen';
                  video.optional = [];
              } else
              if (this.Video == 3) {
                  video.mandatory.chromeMediaSource = 'window';
                  video.optional = [];
              } else
              if (this.Video == 4) {
                  video.mandatory.chromeMediaSource = 'desktop';
                  video.optional = [];
              } else
              if (this.Video == 10) {
                  video.facingMode = 'environment';
              } else
              if (this.Video == 11) {
                  video.facingMode = 'user';
              }
        }
        navigator.mediaDevices.getUserMedia({
            audio: this.Audio,
            video: video
        }).then(function(stream) {
                this.InitStream(stream, false, stream_name);
            }.bind(this)).catch(function(e) {
            alert(this.Tag + ':getUserMedia() error: ' + e.name);
            this.Error(e, "getUserMedia error", stream_name);
        }.bind(this));
    }

    this.NewPeer = function(name, stream_name) {
        var pc = new RTCPeerConnection(this.servers);
        this.Peers[this.Peers.length] = pc;
        pc._ref_owner = this;
        if (!stream_name)
             stream_name = "";
        if (name !== "local")
            this.StreamCount++;
        pc._ref_stream_name = stream_name;
        pc._ref_stream_domain = name;
        pc.onicecandidate = function(e) {
            pc._ref_owner.onIceCandidate(pc, e, name);
        };
        pc.oniceconnectionstatechange = function(e) {
            console.log(pc._ref_owner.Tag + ': ' + name + ' ICE state: ' + pc.iceConnectionState);
            console.log(pc._ref_owner.Tag + ': ICE state change event: ', e);
            if (pc._ref_owner.OnStatusChanged)
                pc._ref_owner.OnStatusChanged(name, pc.iceConnectionState, pc._ref_stream_name);
        };
        if (pc.ontrack === null) {
            pc.ontrack = function(e) {
                if (pc._ref_owner.OnRemoteStream)
                    pc._ref_owner.OnRemoteStream(e.streams[0], pc._ref_stream_name);
            };
        } else {
            pc.onaddstream = function(e) {
                if (pc._ref_owner.OnRemoteStream)
                    pc._ref_owner.OnRemoteStream(e.stream, pc._ref_stream_name);
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
                    this.OnICECandidate(candidate, pc._ref_stream_name);
            }.bind(this),
            this.onAddIceCandidateError.bind(this)
        );
    }

    this.ICE = function(candidate, stream_name) {
        var localPeer = this.GetPeer(stream_name)
        if (!localPeer) {
            alert("ICE: Local peer is not initialized. Call InitMedia first.");
            return;
        }
        localPeer.addIceCandidate(
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

    this.onCreateOfferSuccess = function(desc, stream_name) {
        if (this.OnCreateOffer)
            this.OnCreateOffer(desc, stream_name);
    }

    this.LocalDescription = function(desc, stream_name) {
        var localPeer = this.GetPeer(stream_name)
        if (!localPeer) {
            alert("LocalDescription: Local peer is not initialized. Call InitMedia first.");
            return;
        }
        console.log(this.Tag + ": LOCAL DESCRIPTION");
        localPeer.setLocalDescription(desc).then(function() {  this.onSetLocalSuccess(desc); }.bind(this), this.onSetSessionDescriptionError);
        this.GetPending(stream_name);
    }

    this.onSetRemoteSuccess = function(pc) {
        console.log(this.Tag + ': setRemoteDescription complete');
    }

    this.RemoteDescription = function(desc, stream_name) {
        var localPeer = this.GetPeer(stream_name)
        if (!localPeer) {
            alert("RemoteDescription: Local peer is not initialized. Call InitMedia first.");
            return;
        }
        var local_desc = this.GetPending(stream_name);
        if (local_desc)
            this.LocalDescription(local_desc, stream_name);
        console.log(this.Tag + ": REMOTE DESCRIPTION");
        localPeer.setRemoteDescription(desc).then(function() { this.onSetRemoteSuccess(localPeer); }.bind(this), this.onSetSessionDescriptionError.bind(this));
    }

    this.onCreateAnswerSuccess = function(desc, stream_name) {
        console.log(this.Tag + ": Create anwswer ", desc);
        this.LocalDescription(desc, stream_name);
        if (this.OnCreateAnswer)
            this.OnCreateAnswer(desc, stream_name);
    }

    this.onCreateSessionDescriptionError = function(error) {
        this.Error(error, 'Failed to create session description');
    }

    this.Answer = function(stream_name) {
        var localPeer = this.GetPeer(stream_name)
        if (!localPeer) {
            alert("Answer: Local peer is not initialized. Call InitMedia first.");
            return;
        }
        localPeer.createAnswer().then(function(desc) { this.onCreateAnswerSuccess(desc, stream_name); }.bind(this), this.onCreateSessionDescriptionError.bind(this));
    }

    this.Offer = function(stream_name) {
        var localPeer = this.GetPeer(stream_name)
        if (!localPeer) {
            alert("Offer: Local peer is not initialized. Call InitMedia first.");
            return;
        }
        var pc_remote = this.NewPeer("remote", stream_name);

        var offerOptions = {offerToReceiveAudio: this.Audio ? 1 : 0, offerToReceiveVideo: this.Video ? 1 : 0}; 
        localPeer.createOffer(
            offerOptions
        ).then(function(desc) {
                this.onCreateOfferSuccess(desc, stream_name);
            }.bind(this),
            this.onCreateSessionDescriptionError
        );
        return {"local": localPeer, "remote": pc_remote};
    }

    this.GetPending = function(channel_name) {
        var desc;
        if ((channel_name) && (channel_name.length > 0)) {
            if (this.PendingChannelDescription[channel_name]) {
                desc = this.PendingChannelDescription[channel_name];
                delete this.PendingChannelDescription[channel_name];
            }
        } else
        if (this.PendingDescription) {
            desc = this.PendingDescription;
            delete this.PendingDescription;
        }
        return desc;
    }

    this.AddPending = function(desc, channel_name) {
        if ((channel_name) && (channel_name.length > 0))
            this.PendingChannelDescription[channel_name] = desc;
        else
            this.PendingDescription = desc;
    }

    this.Hangup = function() {
        this.StreamCount = 0;
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
        this.localPeers = { };
        this.PendingChannelDescription = { };
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
